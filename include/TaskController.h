#include <string>

#include "async_task_exec_queue.h"
#include "task_item.h"

namespace migration {

class TaskController {
public:
    explicit TaskController(bool is_toleran_exec, int32_t concurrency);
    ~TaskController();

    void AddTaskItem(TaskItem* task_item);
    void WaitFinish();
    void Cancel();
    void CheckErr();

    void SetErrMsg(const char* err_msg);

private:
    // thread pool
    AsyncTaskExecQueue* async_task_exec_queue_;

    // 容错标识
    bool is_toleran_exec_;

    // error message in thread
    std::string err_msg_;
};  // end class                                                                                                                                                                                 
                                                                                                                                                                                                 
}   // end namespace migration
