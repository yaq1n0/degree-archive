#ifndef task_hpp
#define task_hpp

#include <string>

class Task
{
public:
    virtual ~Task()
    {}

    virtual std::string GetName() const =0;

    virtual double GetNextDeadline() const =0;

    virtual void Execute() =0;
};

#endif
