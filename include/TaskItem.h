#ifndef BE_SRC_MIGRATE_DATA_TOOL_TASK_ITEM_H_
#define BE_SRC_MIGRATE_DATA_TOOL_TASK_ITEM_H_

#include <string>
#include <functional>

namespace migration {

class TaskItem {
public:
    explicit TaskItem(std::function<void(void)> fun);

    void Run();
    std::function<void(const char*)> write_err_msg_fun_;

private:
    std::function<void(void)> fun_;
};  // end class TaskItem

}   // namespace migration

#endif
