#include "migrate_data_tool/task_item.h"
#include <exception>
#include <iostream>
#include "migrate_data_info.h"

namespace migration {
TaskItem::TaskItem(std::function<void(void)> fun) : fun_(fun) {
}

void TaskItem::Run() {
    try {
        fun_();
    } catch (std::exception &e) {
        write_err_msg_fun_(e.what());
    } catch (...) {
        write_err_msg_fun_("unknown error");
    }
}

}   // end namespace migration
