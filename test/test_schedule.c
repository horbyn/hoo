/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/module/sched.h"

__attribute__((aligned(4096))) static uint8_t stack_kt1_r0[PGSIZE] = { 0 },
                                              stack_kt2_r0[PGSIZE] = { 0 },
                                              stack_ut1_r0[PGSIZE] = { 0 },
                                              stack_ut1_r3[PGSIZE] = { 0 };
static uint32_t user_data = 0;

/**
 * @brief kernel mode thread 1
 */
static void
kt1(void) {

    while (1) {
        kprintf("Kernel speaked ");
    }
}

/**
 * @brief kernel mode thread 2
 */
static void
kt2(void) {

    while (1) {
        kprintf("0x%x ", user_data);
    }
}

/**
 * @brief user mode thread 1
 */
static void
ut1(void) {

    while (1) {
        ++user_data;
    }

}

/**
 * @brief scheduling test
 */
void
test_schedule() {
    clear_screen();
    kprintf(">          TEST_SCHEDULE          <\n");

    thread_create(stack_kt1_r0, null, kt1);
    thread_create(stack_kt2_r0, null, kt2);
    thread_create(stack_ut1_r0, stack_ut1_r3, ut1);

}
