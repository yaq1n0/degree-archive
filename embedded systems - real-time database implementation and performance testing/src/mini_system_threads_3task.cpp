
#include "real_time_database_factory.hpp"
//#include "real_time_database.hpp"

#include "tasks/compute_task.hpp"
#include "tasks/output_task.hpp"
#include "tasks/input_task.hpp"

void inputHandler(InputTask task) {
  while(1){
    double tNextDeadline = task.GetNextDeadline();
    wait_until(tNextDeadline);
    task.Execute();
  }
}

void computeHandler(ComputeTask task) {
  while(1){
    double tNextDeadline = task.GetNextDeadline();
    wait_until(tNextDeadline);
    task.Execute();
  }
}

void outputHandler(OutputTask task) {
  while(1){
    double tNextDeadline = task.GetNextDeadline();
    wait_until(tNextDeadline);
    task.Execute();
  }
}

int main()
{
    // TODO : implement me
    auto db=create_real_time_database("locked");

    InputTask inputTask(db.get());
    ComputeTask computeTask(db.get());
    OutputTask outputTask(db.get());

    std::thread t0(inputHandler, inputTask);
    std::thread t1(computeHandler, computeTask);
    std::thread t2(outputHandler, outputTask);

    t0.join();
    t1.join();
    t2.join();
}
