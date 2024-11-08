/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sched.h"
#include "kern/sched/tasks.h"

/**
 * @brief kernel initializes the tasks system
 */
void
kinit_tasks_system(void) {
    init_hoo();
    init_tasks_system();
}
