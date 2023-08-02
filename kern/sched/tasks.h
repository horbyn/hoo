/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

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
    uint32_t ss_;
    uint32_t gs_;
    uint32_t fs_;
    uint32_t es_;
    uint32_t ds_;
} __attribute__ ((packed)) istackos_t;

/**
 * @brief definition of cpu interrupt stack
 */
typedef struct interrupt_stack_cpu {
    uint32_t vec_;
    uint32_t errcode_;
    uint32_t oldeip_;
    uint32_t oldcs_;
    uint32_t eflags_;
} __attribute__ ((packed)) istackcpu_t;

/**
 * @brief definition of Process Control Block
 */
typedef struct pcb {
    uint32_t *thread_stack_;
} pcb_t;

#endif
