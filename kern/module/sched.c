#include "sched.h"
#include "kern/sched/tasks.h"

/**
 * @brief 调度模块初始化
 */
void
kinit_tasks_system(void) {
    init_hoo();
    init_tasks_system();
}
