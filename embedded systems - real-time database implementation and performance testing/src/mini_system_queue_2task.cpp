
#include "real_time_database_factory.hpp"

#include "tasks/compute_task.hpp"
#include "tasks/output_task.hpp"

int main()
{
    auto db=create_real_time_database("naive");

    ComputeTask t1(db.get());
    OutputTask t2(db.get());

    float volume=1000000.0; // Set "volume" key to 100.0 manually
    db->Write("volume", sizeof(float), &volume);

    while(1){
        double tNextSample = std::min( t1.GetNextDeadline(), t2.GetNextDeadline() );
        
        wait_until(tNextSample);

        double tNow=now();
        if(tNow >= t1.GetNextDeadline()){
            t1.Execute();
        } 
        if(tNow >= t2.GetNextDeadline()){
            t2.Execute();
        } 
    }
}