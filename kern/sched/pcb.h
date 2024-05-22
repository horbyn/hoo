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
#include "kern/units/bitmap.h"
#include "kern/units/sleeplock.h"

// switch when the amount arrived
#define TIMETICKS   16
#define TID_HOO     0
#define TID_IDLE    1

/**
 * @brief paging struct
 */
typedef struct paging_struct {
    // virtual address of this thread page directory table
    void *pdir_va_;
    // physical address of this thread page directory table
    void *pdir_pa_;
    // a 4KB buffer to record all virtual address of the page tables
    pgelem_t *mapping_;
} __attribute__((packed)) pgstruct_t;

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
    pgstruct_t   pgstruct_;
    // virtual space manager
    vsmngr_t     vmngr_;
    // the rest ticks
    uint32_t     ticks_;
    // the task was slept if it is not null
    sleeplock_t  *sleeplock_;
} __attribute__((packed)) pcb_t;

void  pgstruct_set(pgstruct_t *pgs, void *pgdir_va, void *pgdir_pa,
    pgelem_t *mapping);
void  pcb_set(uint32_t *scur, uint32_t *s0, uint32_t tid, pgstruct_t *pgs,
    void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace,
    uint32_t ticks, sleeplock_t *sleeplock);
pcb_t *pcb_get(tid_t tid);
void  init_pcb_system(void);
tid_t allocate_tid(void);

#endif
