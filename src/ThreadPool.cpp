#include "migrate_data_tool/async_task_exec_queue.h"
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "common/logging.h"

namespace migration {

AsyncTaskExecQueue::AsyncTaskExecQueue(int32_t concurrency)
        :mutex_(), mutext_finish_(), cond_var_(), cond_finish_(), residual_task_num_(0) {
    is_started_ = true;
    threads.reserve(concurrency);
    for (int i = 0; i < concurrency; ++i) {
        threads.push_back(new std::thread(std::bind(&AsyncTaskExecQueue::WorkThreadFunc, this)));
    }
}

AsyncTaskExecQueue::~AsyncTaskExecQueue() {
    LOG(INFO) << "AsyncTaskExecQueue::stop() called";
    if (is_started_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            is_started_ = false;
            cond_var_.notify_all();
            LOG(INFO) << "AsyncTaskExecQueue::stop() notifyall";
        }
        for (Threads::iterator it = threads.begin(); it != threads.end(); it ++) {
            (*it)->join();
            delete *it;
        }                                                                                                                                                                                        
        threads.clear();                                                                                                                                                                         
    }                                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void AsyncTaskExecQueue::WaitFinish() {                                                                                                                                                          
    LOG(INFO) << "AsyncTaskExecQueue::WaitFinish start";                                                                                                                                         
    if (is_started_ == false) {                                                                                                                                                                  
        return;                                                                                                                                                                                  
    }                                                                                                                                                                                            
                                                                                                                                                                                                 
    {                                                                                                                                                                                            
        std::unique_lock<std::mutex> lock(mutext_finish_);                                                                                                                                       
        // the count of task unprocessed                                                                                                                                                         
        if (residual_task_num_ == 0) {                                                                                                                                                           
            LOG(INFO) << "AsyncTaskExecQueue::WaitFinish exit, residual_task_num_ is 0";                                                                                                         
            return;                                                                                                                                                                              
        }                                                                                                                                                                                        
        LOG(INFO) << "AsyncTaskExecQueue::WaitFinish finish before wait";                                                                                                                        
        cond_finish_.wait(lock);                                                                                                                                                                 
        LOG(INFO) << "AsyncTaskExecQueue::WaitFinish finish"; 
         }                                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void AsyncTaskExecQueue::WorkThreadFunc() {                                                                                                                                                      
    LOG(INFO) << "AsyncTaskExecQueue::WorkThreadFunc() tid : "                                                                                                                                   
            << std::this_thread::get_id() << " start";                                                                                                                                           
    while (is_started_) {                                                                                                                                                                        
        LOG(INFO) << "AsyncTaskExecQueue::WorkThreadFunc while .....";                                                                                                                           
        TaskItem* task_item = Take();                                                                                                                                                            
        if (task_item) {                                                                                                                                                                         
            task_item->Run();                                                                                                                                                                    
            {                                                                                                                                                                                    
                std::unique_lock<std::mutex> lock(mutext_finish_);                                                                                                                               
                // count -1                                                                                                                                                                      
                residual_task_num_--;                                                                                                                                                            
                if (residual_task_num_ == 0) {                                                                                                                                                   
                    LOG(INFO) << "AsyncTaskExecQueue::WorkThreadFunc all task is finished";                                                                                                      
                    cond_finish_.notify_one();                                                                                                                                                   
                }                                                                                                                                                                                
            }                                                                                                                                                                                    
        }                                                                                                                                                                                        
    }                                                                                                                                                                                            
    LOG(INFO) << "AsyncTaskExecQueue::WorkThreadFunc tid : "                                                                                                                                     
            << std::this_thread::get_id() << " end";                                                                                                                                             
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void AsyncTaskExecQueue::Push(TaskItem* task_item_) {                                                                                                                                            
    {                                                                                                                                                                                            
        std::unique_lock<std::mutex> lock(mutex_);                                                                                                                                               
        tasks_.push(task_item_);                                                                                                                                                                 
        cond_var_.notify_one();                                                                                                                                                                  
    }                                                                                                                                                                                            
    {                                                                                                                                                                                            
        std::unique_lock<std::mutex> lock(mutext_finish_);                                                                                                                                       
        residual_task_num_++;                                                                                                                                                                    
    }                                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void AsyncTaskExecQueue::Cancel() {                                                                                                                                                              
    LOG(INFO) << "AsyncTaskExecQueue::Cancel() called";                                                                                                                                          
    is_started_ = false;                                                                                                                                                                         
}                                                                                                                                                                                                
                                                                                                                                                                                                 
TaskItem* AsyncTaskExecQueue::Take() {                                                                                                                                                           
    std::unique_lock<std::mutex> lock(mutex_);                                                                                                                                                   
    // always use a while-loop, due to spurious wakeup                                                                                                                                           
    while (tasks_.empty() && is_started_) {                                                                                                                                                      
        // LOG(INFO) << "AsyncTaskExecQueue::Take() tid : " << std::this_thread::get_id() << " wait.";                                                                                           
        cond_var_.wait(lock);                                                                                                                                                                    
    }
    TaskItem* task_item = NULL;                                                                                                                                                                  
    std::queue<TaskItem*>::size_type size = tasks_.size();                                                                                                                                       
    if (!tasks_.empty() && is_started_) {                                                                                                                                                        
        task_item = tasks_.front();                                                                                                                                                              
        tasks_.pop();                                                                                                                                                                            
        assert(size - 1 == tasks_.size());                                                                                                                                                       
        LOG(INFO) << "AsyncTaskExecQueue::Take() one task was Taken";                                                                                                                            
    }                                                                                                                                                                                            
    return task_item;                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
}   // namespace migration
