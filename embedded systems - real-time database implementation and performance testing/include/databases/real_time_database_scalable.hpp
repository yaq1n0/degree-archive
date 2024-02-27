#ifndef real_time_database_scalable_hpp
#define real_time_database_scalable_hpp

#include "real_time_database.hpp"
#include <shared_mutex>

class RealTimeDatabaseScalable
    : public RealTimeDatabase
{
private:
    std::shared_mutex mtx;
    std::unordered_map<std::string, std::vector<char>> entries;
public:
    std::string GetName() const override
    { return "scalable"; }

    bool IsThreadSafe() const override
    { return true; }

    // only called at init, concurrency shouldn't be an issue
    intptr_t Register(const char *name, size_t size) override
    {

        if (entries.find(name) == entries.end()) {
          // name not found in entries, make a new key, value pair
          // make a temporary pair of name and empty data vector
          // then insert the pair into entries map
          auto tmp_pair = std::make_pair(name, std::vector<char>(size, 0));
          entries.insert(tmp_pair);
          return 0;
        } else {
          // found, assert that value vector size is correct
          assert(entries.at(name).size() == size);
          return 0;
        }
    }

    void Read(const char *name, intptr_t token, size_t size, void *data) override
    {
        if (entries.find(name) == entries.end()) {
          // name not found in entries, return 0
          memset(data, 0, size);
        } else {
          // found, assert value size is correct (for some reason), copy value to data
          mtx.lock_shared(); // request shared lock
          assert(entries.at(name).size() == size);
          memcpy(data, &entries.at(name)[0], size);
          mtx.unlock_shared();
        }
    }

    void Write(const char *name, intptr_t token, size_t size, const void *data) override
    {

        if (entries.find(name) == entries.end()) {
          /// name not found in entries, make a new key, value pair
          // make a temporary pair of name and empty data vector
          // then insert the pair into entries map
          mtx.lock();
          auto tmp_pair = std::make_pair(name, std::vector<char>(size, 0));
          entries.insert(tmp_pair);
          mtx.unlock();
        } else {
          // found, assert value size is correct (for some reason), copy data to value
          mtx.lock();
          assert(entries.at(name).size() == size);
          memcpy(&entries.at(name)[0], data, size);
          mtx.unlock();
        }
    }
};

#endif
