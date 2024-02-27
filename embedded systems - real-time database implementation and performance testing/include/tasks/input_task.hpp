#ifndef input_task_hpp
#define input_task_hpp

#include "task.hpp"
#include "real_time_database.hpp"

#include <cstdio>

class InputTask
    : public Task
{
private:
    RealTimeDatabase *m_db;
    std::string key;
    intptr_t token;
    float value;

    double m_t_next;
public:
    InputTask(RealTimeDatabase *db)
    {
        // Disable buffering, so that characters are read without pressing return
        //setvbuf(stdin, NULL, _IONBF, 1);
        setbuf(stdin, NULL);

        m_db=db;
        key="volume";
        token=db->Register(key, sizeof(float));
        value=1.0;
        db->Write(key, token, sizeof(value), &value);

        m_t_next=now();
    }

    virtual std::string GetName() const
    { return "input"; }

    virtual double GetNextDeadline() const
    {
        return m_t_next;
    }

    virtual void Execute()
    {
        char x=getchar();
        if(x=='+'){
            value *= 1.1;
            value=std::min<double>(value, 1.0e8);
            fprintf(stderr, "New volume = %g\n", value);
        }else if(x=='-'){
            value /= 1.1;
            value=std::max<double>(value, 1.0);
            fprintf(stderr, "New volume = %g\n", value);
        }else if(x=='q'){
            exit(0);
        }
        m_db->Write(key, token, sizeof(value), &value);

        m_t_next=now()+0.1;
    }
};

#endif
