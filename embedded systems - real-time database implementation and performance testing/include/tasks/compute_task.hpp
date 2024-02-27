#ifndef compute_task_hpp
#define compute_task_hpp

#include "task.hpp"
#include "real_time_database.hpp"

#include "time_helpers.hpp"

#include <cstdio>
#include <random>

class ComputeTask
    : public Task
{
private:
    RealTimeDatabase *m_db;

    std::string m_volume_key;
    intptr_t m_volume_token;

    std::string m_result_key;
    intptr_t m_result_token;

    std::mt19937_64 m_rng;

    double m_next_time;
public:
    ComputeTask(RealTimeDatabase *db)
    {
        m_db=db;

        m_volume_key="volume";
        m_volume_token=db->Register(m_volume_key, sizeof(float));

        m_result_key="result";
        m_result_token=db->Register(m_result_key, sizeof(float));

        m_next_time = now() + 0.1;
    }

    virtual std::string GetName() const
    { return "input"; }

    virtual double GetNextDeadline() const
    { return m_next_time; }

    virtual void Execute()
    {
        float volume;
        m_db->Read(m_volume_key, m_volume_token, sizeof(volume), &volume);
        assert(volume>0);

        std::exponential_distribution<> edist;

        double length=edist(m_rng) * volume;
        
        float result=1;
        for(double i=0; i<length; i++){
            result += sin(i);
        }
        //std::cerr<<"L="<<length<<", volume="<<volume<<"\n";

        m_db->Write(m_result_key, m_result_token, sizeof(result), &result);

        m_next_time = now() + 0.1; // At most 10 key-strokes per second
    }
};

#endif
