/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "tasks_test.h"

static uint8_t __init_stack[PGSIZE] = { 0 };                // the stack used by init thread
static uint8_t __init_stack_r3[PGSIZE] = { 0 };

/**
 * @brief user mode thread
 */
void
init_thread() {
    uint32_t user = 0;

    while (1) {
        printf("%x ", ++user);
    }
}

/**
 * @brief kernel mode thread
 */
static void
idle_test(void) {

    while (1) {
        kprintf("Kernel ");
    }
}

/**
 * @brief test kernel threads if works well
 */
void
test_kernel_threads() {
    create_kthread(__init_stack, __init_stack_r3, init_thread);

    idle_test();
}
