/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SCHED_PCB_H__
#define __KERN_SCHED_PCB_H__

#include "kern/x86.h"
#include "kern/conf/page.h"
#include "kern/mem/vaddr.h"

// switch when the amount arrived
#define TIMETICKS   16

/**
 * @brief definition of Process Control Block
 */
typedef struct pcb {
    // current stack that either kernel stack or user stack
    uint32_t     *stack_cur_;
    // kernel stack
    uint32_t     *stack0_;
    // thread id
    tid_t        tid_;
    // virtual address of this thread page directory table
    void         *pdir_va_;
    // virtual space manager
    vsmngr_t     vmngr_;
    // the rest ticks
    uint32_t     ticks_;
} __attribute__((packed)) pcb_t;

void  pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, void *va_pdir,
    void *va_vspace, void *va_node, void *va_vaddr, uint32_t ticks);

#endif
