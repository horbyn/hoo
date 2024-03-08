/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SYSCALL_STUFFSYSC_H__
#define __KERN_SYSCALL_STUFFSYSC_H__

#include "kern/types.h"
#include "kern/lib/lib.h"
#include "kern/disp/disp.h"

#define MAX_SYSCALL 32
typedef void (*syscall_t)(void);
extern syscall_t __stub[MAX_SYSCALL];

/**
 * @brief general regs for user process to be recorded
 * when entering library function
 */
typedef struct general_registers {
    uint32_t edi_;
    uint32_t esi_;
    uint32_t ebp_;
    uint32_t esp_;
    uint32_t ebx_;
    uint32_t edx_;
    uint32_t ecx_;
    uint32_t eax_;
} __attribute__ ((packed)) gregs_t;

/**
 * @brief eflags for user process to be recorded
 * when entering library function
 */
typedef uint32_t eflags_t;

void stash_user_context(gregs_t *, eflags_t *);
void restore_user_context(gregs_t *, eflags_t *);

#endif
