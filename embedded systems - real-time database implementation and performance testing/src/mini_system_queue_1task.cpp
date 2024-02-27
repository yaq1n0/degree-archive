
#include "real_time_database_factory.hpp"

#include "tasks/output_task.hpp"

int main()
{
    auto db=create_real_time_database("naive");

    OutputTask out(db.get());

    while(1){
        double tNextDeadline = out.GetNextDeadline();
        wait_until(tNextDeadline);
        out.Execute();
    }
}
