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
    kernel_init_thread();
}

/**
 * @brief locks resource initialization
 */
void
init_locks() {
    // kern/disp/disp.h
    spinlock_init(&__spinlock_disp);
}
