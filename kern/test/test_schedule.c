/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/module/sched.h"

/**
 * @brief scheduling test
 */
void
test_schedule() {
    clear_screen();
    kprintf(">          TEST_SCHEDULE          <\n");

    tid_t t1 = fork();
    kprintf("  tid %d created\n", t1);
}
