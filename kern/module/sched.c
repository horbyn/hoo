/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "sched.h"

/**
 * @brief test if scheduler works well
 */
void
test_scheduler() {
    init_tasks_queue();
    kernel_idle_thread();
    kernel_init_thread();
}
