/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "sched.h"

/**
 * @brief kernel initializes the tasks system
 */
void
kinit_tasks_system(void) {
    init_tasks_system();
}

/**
 * @brief create idle thread
 */
void
kinit_idle_thread(void) {
    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // idle thread, and the stack it used is idle stack
    __pcb_idle = (pcb_t *)STACK_BOOT_BOTTOM;
    tid_t idle_tid = allocate_tid();
    pcb_set(__pcb_idle, null, (uint32_t *)STACK_BOOT_TOP, idle_tid,
        __pgdir_idle, TIMETICKS);
    queue_t *running_queue = get_idle_running_queue();
    if (running_queue == null)    panic("kinit_idle_thread(): bug");
    static node_t n;
    node_set(&n, __pcb_idle, null);
    queue_push(running_queue, &n, TAIL);

}
