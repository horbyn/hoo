/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_INTE_STACK_H__
#define __KERN_SCHED_INTE_STACK_H__

#include "kern/types.h"

/**
 * @brief definition of os interrupt stack corresponding
 * the order of storing context in `kern/inter/trampoline.s`
 */
typedef struct interrupt_stack_os {
    uint32_t edi_;
    uint32_t esi_;
    uint32_t ebp_;
    uint32_t esp_;
    uint32_t ebx_;
    uint32_t edx_;
    uint32_t ecx_;
    uint32_t eax_;
    uint32_t gs_;
    uint32_t fs_;
    uint32_t es_;
    uint32_t ds_;
} __attribute__ ((packed)) istackos_t;

/**
 * @brief definition of cpu interrupt stack that builds
 * the env the cpu executes `iret` instruction
 */
typedef struct interrupt_stack_cpu {
    uint32_t vec_;                                          // unused when `iret`ing
    uint32_t errcode_;                                      // unused when `iret`ing
    void    *oldeip_;
    uint32_t oldcs_;
    uint32_t eflags_;

} __attribute__ ((packed)) istackcpu_t;

/**
 * @brief thread stack
 */
typedef struct thread_stack {
    void *retaddr_;
} tstack_t;

#endif
