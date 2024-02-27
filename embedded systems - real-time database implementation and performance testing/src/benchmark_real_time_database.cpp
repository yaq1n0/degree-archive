#include "real_time_database_factory.hpp"

#include <unordered_set>
#include <atomic>
#include <random>
#include <iostream>

#include "time_helpers.hpp"

/* Generate a unique name in a thread-safe way.
*/
std::string make_unique_name()
{
    static std::atomic<uint64_t> names_seed; // Rely on static zero initialisation

    uint64_t seed=names_seed.fetch_add(19937);

    return "key_"+std::to_string(seed);
}

template<class TRng>
uint8_t fill_random_and_checksum(void *dst, size_t count, TRng &&rng)
{
    // Handle first batch of 64-bit values, avoiding lots of calls to rng
    unsigned full64=count/8;
    auto dst64 = (uint64_t *)dst;
    uint64_t checksum=0; // Four 16-bit checksums
    for(unsigned i=0; i<full64; i++){
        uint64_t x=rng();
        dst64[i] = x;
        checksum = (checksum + (x&0x00FF00FF00FF00FFull) + ((x>>8)&0x00FF00FF00FF00FFull)) & 0x00FF00FF00FF00FFull;
    }
    count -= full64*8;
    dst64 += full64;

    checksum = (checksum + (checksum >> 32)) & 0x00FF00FFul;
    checksum = (checksum + (checksum >> 16)) & 0xFF;

    // Handle final part carefully to avoid overwriting other bytes
    if(count > 0){
        uint64_t x=rng();
        auto dst8=(uint8_t *)dst64;
        while(count>0){
            uint8_t tmp=x&0xFF;
            *dst8++ = x&0xFF;
            x >>= 8;
            --count;
            checksum += tmp;
        }
    }

    return checksum;
}

template<class TRng>
void randomise_with_checksum(std::vector<uint8_t> &data, TRng &&rng)
{
    uint8_t checksum = fill_random_and_checksum(&data[0], data.size()-1, rng);
    data.back() = checksum;
}

template<class TRng>
std::vector<uint8_t> random_value_with_checksum(TRng &&rng)
{
    uint64_t len = 4 + (rng() % (128-4)); // Value lengths in range [4,128) bytes
    std::vector<uint8_t> res(len);
    randomise_with_checksum(res, rng);
    return res;
}


bool validate_checksum(const std::vector<uint8_t> &data)
{
    uint8_t sum=0;
    for(unsigned i=0; i<data.size()-1; i++){
        sum += data[i];
    }
    return data.back() == sum;
}


struct test_parameters
{
    unsigned numAccess = 10000000; // Number of accesses in total, across all threads
    unsigned numEntries = 100;
    unsigned numThreads = 1;
    double probWrite = 0.5;    // Probability that access is a write
    uint64_t seed = 1;

    static void write_header(FILE *dst)
    {
        fprintf(dst, "Accesses,Entries,Threads");
    }

    void write(FILE *dst) const
    {
        fprintf(dst, "%u,%u,%u", numAccess, numEntries, numThreads);
    }

    void write_key_value(FILE *dst) const
    {
        fprintf(dst, "NumAccess=%u,NumEntries=%u,NumThreads=%u", numAccess, numEntries, numThreads);
    }
};

struct test_results
{
    test_parameters parameters;
    std::string name;  // Name of the implementation
    unsigned numChecksumFailures;
    unsigned hardwareConcurrency; // Result of std::hardware_concurrency
    double wallTime;    // Wall-clock time to execute benchmark

    static void write_header(FILE *dst)
    {
        fprintf(dst, "Database,");
        test_parameters::write_header(dst);
        fprintf(dst, ",CheckSumFailures,HWThreads,WallTime");
    }

    void write(FILE *dst) const
    {
        fprintf(dst,"%s,",name.c_str());
        parameters.write(dst);
        fprintf(dst, ",%u,%u,%g", numChecksumFailures, hardwareConcurrency, wallTime);
    }
};

test_results test_workload( RealTimeDatabase *db, const test_parameters &parameters)
{
    struct entry{
        std::string name;
        size_t size;
        intptr_t token;
    };

    std::vector<entry> entries;

    std::mt19937_64 rng(parameters.seed);
    std::uniform_real_distribution<> udist;

    for(unsigned i=0; i<parameters.numEntries; i++){
        std::string name=make_unique_name();
        std::vector<uint8_t> value=random_value_with_checksum(rng);
        intptr_t token=db->Register(name.c_str(), value.size());
        entries.push_back({ name, value.size(), token});
        db->Write(name.c_str(), token, value.size(), &value[0]);
    }

    std::atomic<unsigned> checksum_failures;
    checksum_failures.store(0);

    auto mutator = [&](unsigned thread_offset)
    {
        std::mt19937_64 rng(parameters.seed + thread_offset);

        std::vector<uint8_t> tmp;
        for(unsigned i=0; i<parameters.numAccess/parameters.numThreads; i++){
            auto &e = entries[ rng() % parameters.numEntries ];

            tmp.resize(e.size);
            db->Read( e.name.c_str(), e.token, tmp.size(), &tmp[0] );

            if(!validate_checksum(tmp)){
                checksum_failures.fetch_add(1, std::memory_order_relaxed);
            }

            if(udist(rng) < parameters.probWrite){
                randomise_with_checksum(tmp, rng);
                db->Write( e.name.c_str(), e.token, tmp.size(), &tmp[0] );
            }
        }
    };


    std::vector<std::thread> threads;

    double startTime=now();

    for(unsigned i=0; i<parameters.numThreads; i++){
        auto mutator_i = [&,i](){ mutator(i); };
        threads.push_back(std::thread(mutator_i));
    }

    for(unsigned i=0; i<parameters.numThreads; i++){
        threads[i].join();
    }

    double endTime=now();

    test_results res;
    res.parameters=parameters;
    res.name=db->GetName();
    res.hardwareConcurrency=std::thread::hardware_concurrency();
    res.numChecksumFailures=checksum_failures.load();
    res.wallTime=endTime-startTime;

    fprintf(stderr, "Found %u failures in %u updates across %u threads.\n", res.numChecksumFailures, parameters.numAccess, parameters.numThreads);

    return res;
}

int main(int argc, char *argv[])
{
    test_parameters params;

    std::string name="naive";
    if(argc>1){
        name=argv[1];
    }
    if(argc>2){
        params.numAccess=std::stoi(argv[2]);
    }
    if(argc>3){
        params.numEntries=std::stoi(argv[3]);
    }
    if(argc>4){
        params.numThreads=std::stoi(argv[4]);
    }
    params.probWrite=0.5;
    params.seed=time(0);

    fprintf(stderr, "Name=%s, ", name.c_str());
    params.write_key_value(stderr);
    fputs("\n", stderr);

    auto db=create_real_time_database(name);


    if(params.numThreads>1 && !db->IsThreadSafe()){
        fprintf(stderr, "Refusing to run with numThreads>1 as database %s has IsThreadSafe()==0", db->GetName().c_str());
        exit(1);
    }
    auto res=test_workload(db.get(), params);

    test_results::write_header(stdout);
    fputs("\n", stdout);
    res.write(stdout);
    fputs("\n", stdout);
}
