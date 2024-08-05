/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/module/sched.h"
#include "user/user.h"

/**
 * @brief scheduling test
 */
void
test_schedule() {
    clear_screen();
    kprintf(">          TEST_SCHEDULE          <\n");

    tid_t t1;
    __asm__ ("mov $fork1, %%eax\n\r"
        "push %%eax\n\r"
        "call fork\n\r"
        "addl $4, %%esp\n\r"
        "fork1:\n\r"
        "movl %%eax, %0\n\r" : "=r"(t1) :);
    if (t1 == 0) {
        sys_printf("  i am the child one\n");
        while (1);
    } else    kprintf("  tid %d created\n", t1);

    tid_t t2;
    __asm__ ("mov $fork2, %%eax\n\r"
        "push %%eax\n\r"
        "call fork\n\r"
        "addl $4, %%esp\n\r"
        "fork2:\n\r"
        "movl %%eax, %0\n\r" : "=r"(t2) :);
    if (t2 == 0) {
        sys_printf("  i am the child two\n");
        while (1);
    } else    kprintf("  tid %d created\n", t2);

    tid_t t3;
    __asm__ ("mov $fork3, %%eax\n\r"
        "push %%eax\n\r"
        "call fork\n\r"
        "addl $4, %%esp\n\r"
        "fork3:\n\r"
        "movl %%eax, %0\n\r" : "=r"(t3) :);
    if (t3 == 0) {
        sys_printf("  i am the child three\n");
        while (1);
    } else    kprintf("  tid %d created\n", t3);

}
