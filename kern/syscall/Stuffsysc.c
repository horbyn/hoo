/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "Stuffsysc.h"

/**
 * @brief system call stub array
 */
syscall_t __stub[MAX_SYSCALL] = {
    (syscall_t)kprintf,                                     // #0

    // MUST BE null terminated
    0
};

/**
 * @brief stash user context when invoke system call
 * @param gr general registers
 * @param ef eflags
 */
void
stash_user_context(gregs_t *gr, eflags_t *ef) {
    if (gr) {
        gregs_t tmp;
        __asm__ ("pushal\n\t"
            "movl %%esp, %0" : "=m"(tmp) ::);
        memmove(gr, &tmp, sizeof(gregs_t));
        __asm__ ("popal");
    }
    if (ef) {
        eflags_t tmp;
        __asm__ ("pushf\n\t"
            "movl %%esp, %0" : "=m"(tmp) ::);
        memmove(ef, &tmp, sizeof(eflags_t));
        __asm__ ("popf");
    }
}

/**
 * @brief restore user context when invoke system call
 * @param gr general registers
 * @param ef eflags
 */
void
restore_user_context(gregs_t *gr, eflags_t *ef) {
    if (gr) {
        uint32_t tmp_esp = 0;
        __asm__ ("movl %%esp, %0\n\t"
            "movl %1, %%esp\n\t"
            "popal" : "=m"(tmp_esp) : "m"(gr) :);
        __asm__ ("movl %0, %%esp" :: "m"(tmp_esp) :);
    }
    if (ef) {
        __asm__ ("pushl %0\n\t"
            "popf" :: "m"(ef) :);
    }
}
