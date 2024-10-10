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
#include "kern/mem/bucket_mngr.h"
#include "kern/mem/vaddr.h"
#include "kern/fs/fmngr.h"
#include "kern/units/bitmap.h"
#include "kern/units/sleeplock.h"

// switch when the amount arrived
#define TIMETICKS   16
#define TID_HOO     0
#define TID_IDLE    1

/**
 * @brief definition of Process Control Block
 */
typedef struct pcb {
    // current stack that either kernel stack or user stack
    uint32_t     *stack0_;
    // user stack
    uint32_t     *stack3_;
    // thread id
    tid_t        tid_;
    pgelem_t     *pgdir_pa_;
    // virtual space manager
    vsmngr_t     vmngr_;
    // the rest ticks
    uint32_t     ticks_;
    // the task was slept if it is not null
    sleeplock_t  *sleeplock_;
    // for heap memory allocation
    buckx_mngr_t *hmngr_;
    fmngr_t      fmngr_;
    // the end of all segments
    uint32_t     break_;
    tid_t        parent_;
    // current directory
    char *       dir_;
    uint32_t     dirlen_;
} __attribute__((packed)) pcb_t;

void pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid,
    pgelem_t *pd_pa, void *va_vspace, void *va_node, void *va_vaddr,
    vspace_t *vspace, uint32_t ticks, sleeplock_t *sleeplock,
    buckx_mngr_t *bucket, fmngr_t *fmngr, uint32_t brk, tid_t parent,
    char *dir, uint32_t dirlen);

#endif
