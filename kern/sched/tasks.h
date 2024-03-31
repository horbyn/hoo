/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

#include "kern/x86.h"
#include "kern/mem/vaddr.h"

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
    vspace_t     vspace_;
} __attribute__((packed)) pcb_t;

void  pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, void *pdir_va);
tid_t pcb_get_tid(pcb_t *pcb);
pcb_t *get_current_pcb(void);
void  init_tasks_system(void);
tid_t tid_get(void);

#endif
