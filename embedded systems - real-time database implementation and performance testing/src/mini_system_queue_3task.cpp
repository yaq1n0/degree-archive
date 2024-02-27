
#include "real_time_database_factory.hpp"

#include "tasks/compute_task.hpp"
#include "tasks/output_task.hpp"
#include "tasks/input_task.hpp"

int main()
{
    auto db=create_real_time_database("naive");

    InputTask t0(db.get());
    ComputeTask t1(db.get());
    OutputTask t2(db.get());

    while(1){
        double tNextSample = std::min(t0.GetNextDeadline(), std::min( t1.GetNextDeadline(), t2.GetNextDeadline() ));
        
        wait_until(tNextSample);

        double tNow=now();
        if(tNow >= t0.GetNextDeadline()){
            t0.Execute();
        } 
        if(tNow >= t1.GetNextDeadline()){
            t1.Execute();
        } 
        if(tNow >= t2.GetNextDeadline()){
            t2.Execute();
        } 
    }
}