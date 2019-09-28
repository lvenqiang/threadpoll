#include "migrate_data_tool/task_controller.h"
#include "MigrateCommon.h"
#include "common/logging.h"

namespace migration {
TaskController::TaskController(bool is_toleran_exec, int32_t concurrency) :
        is_toleran_exec_(is_toleran_exec) {
    async_task_exec_queue_ = new AsyncTaskExecQueue(concurrency);
    err_msg_.clear();
}

TaskController::~TaskController() {
    LOG(INFO) << "TaskController destruct was called";
    if (async_task_exec_queue_ != NULL) {
        delete async_task_exec_queue_;
        async_task_exec_queue_ = NULL;
    }
}

void TaskController::AddTaskItem(TaskItem* task_item) {
    task_item->write_err_msg_fun_ = std::bind(&TaskController::SetErrMsg, this, std::placeholders::_1);
    async_task_exec_queue_->Push(task_item);
}

void TaskController::WaitFinish() {
    async_task_exec_queue_->WaitFinish();
}

void TaskController::Cancel() {
    async_task_exec_queue_->Cancel();                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void TaskController::SetErrMsg(const char* err_msg) {                                                                                                                                            
    err_msg_ = err_msg;                                                                                                                                                                          
    if (!is_toleran_exec_) {                                                                                                                                                                     
        async_task_exec_queue_->Cancel();                                                                                                                                                        
    }                                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
void TaskController::CheckErr() {                                                                                                                                                                
    if (!err_msg_.empty()) {                                                                                                                                                                     
        LOG(ERROR) << "error occured while migrate data : " << err_msg_;                                                                                                                         
        throw MGRException(err_msg_);                                                                                                                                                            
    }                                                                                                                                                                                            
}                                                                                                                                                                                                
                                                                                                                                                                                                 
}   // namespace migration
