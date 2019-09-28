#ifndef ASYNC_TASK_EXEC_QUEUE_H_
#define ASYNC_TASK_EXEC_QUEUE_H_

#include "migrate_data_tool/task_item.h"

#include <vector>
#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace migration {

class AsyncTaskExecQueue{
public:
    explicit AsyncTaskExecQueue(int32_t concurrency);
    ~AsyncTaskExecQueue();

    void Push(TaskItem* task_item);
    void Cancel();

    void WaitFinish();

private:
    AsyncTaskExecQueue(const AsyncTaskExecQueue& threadPool);   // 禁止拷贝
    const AsyncTaskExecQueue& operator=(const AsyncTaskExecQueue&);

    // get taskItem* from task queue                                                                                                                                                             
    TaskItem* Take();                                                                                                                                                                            
                                                                                                                                                                                                 
    // the function for thread                                                                                                                                                                   
    void WorkThreadFunc();                                                                                                                                                                       
                                                                                                                                                                                                 
private:                                                                                                                                                                                         
    // the number of tasks not finish                                                                                                                                                            
    int residual_task_num_;                                                                                                                                                                      
                                                                                                                                                                                                 
    typedef std::vector<std::thread*> Threads;                                                                                                                                                   
    Threads threads;                                                                                                                                                                             
                                                                                                                                                                                                 
    std::queue<TaskItem*> tasks_;                                                                                                                                                                
                                                                                                                                                                                                 
    std::mutex mutext_finish_;                                                                                                                                                                   
    std::mutex mutex_;                                                                                                                                                                           
    std::condition_variable cond_finish_; 
    // started indicate                                                                                                                                                                          
    std::atomic<bool> is_started_;                                                                                                                                                               
};                                                                                                                                                                                               
                                                                                                                                                                                                 
}   // namespace migration                                                                                                                                                                       
                                                                                                                                                                                                 
#endif
