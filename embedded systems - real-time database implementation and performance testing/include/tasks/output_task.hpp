#ifndef output_task_hpp
#define output_task_hpp

#include "task.hpp"
#include "real_time_database.hpp"

#include "jitter_tracker.hpp"

#include <cstdio>

class OutputTask
    : public Task
{
private:
    RealTimeDatabase *m_db;

    std::string m_result_key;
    intptr_t m_result_token;

    std::string m_volume_key;
    intptr_t m_volume_token;

    double m_d_sample;

    double m_nextExecute;
    double m_nextPrint;

    JitterTracker m_jitter;
public:
    OutputTask(RealTimeDatabase *db)
    {
        m_db=db;

        m_d_sample = 0.01;

        m_volume_key="volume";
        m_volume_token=db->Register(m_volume_key, sizeof(float));

        m_result_key="result";
        m_result_token=db->Register(m_result_key, sizeof(float));

        m_nextExecute=now()+m_d_sample;
        m_nextPrint=now()+1;

        JitterTracker::print_header(std::cout);
        std::cout<<",Volume,Result\n";

        m_jitter.Reset(m_d_sample);
    }

    virtual std::string GetName() const
    { return "output"; }

    virtual double GetNextDeadline() const
    {
        return m_nextExecute;
    }

    virtual void Execute()
    {
        double t=now();
        assert(t >= m_nextExecute);

        m_jitter.OnExecute(t, m_nextExecute);
        
        if(t >= m_nextPrint){
            float volume, result;
            m_db->Read(m_volume_key, m_volume_token, sizeof(volume), &volume);
            m_db->Read(m_result_key, m_result_token, sizeof(result), &result);

            std::cout << m_jitter.CollectStats() << ",  "<<volume<<", "<<result<< std::endl;
            m_nextPrint += 1;
        }

        m_nextExecute += m_d_sample;
    }
};

#endif
