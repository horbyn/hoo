/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "sched.h"

/**
 * @brief scheduler initialization
 */
void
init_scheduler() {

    init_tasks_queue();
    kernel_idle_thread();
    user_init_thread();
}
