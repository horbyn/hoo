/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/module/sched.h"

/**
 * @brief kernel mode thread 1
 */
static void
t1(void) {

    while (1) {
        kprintf("1111 ");
    }
}

/**
 * @brief kernel mode thread 2
 */
static void
t2(void) {

    while (1) {
        kprintf("2222_2222 ");
    }
}

/**
 * @brief scheduling test
 */
void
test_schedule() {
    clear_screen();
    kprintf(">          TEST_SCHEDULE          <\n");

    static uint8_t stack_t1_r0[PGSIZE] = { 0 }, stack_t2_r0[PGSIZE] = { 0 };
    kthread_create(stack_t1_r0, null, t1);
    kthread_create(stack_t2_r0, null, t2);

}
