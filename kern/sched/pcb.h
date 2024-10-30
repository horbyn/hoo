/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SCHED_PCB_H__
#define __KERN_SCHED_PCB_H__

#include "kern/types.h"
#include "kern/fs/fmngr.h"
#include "kern/mem/vspace.h"
#include "kern/mem/bucket.h"
#include "kern/page/page_stuff.h"

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
    vspace_t     vmngr_;
    // the rest ticks
    uint32_t     ticks_;
    // for heap memory allocation
    buckx_mngr_t *hmngr_;
    fmngr_t      *fmngr_;
    // the end of all segments
    uint32_t     break_;
} pcb_t;

void pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
    vspace_t *vmngr, uint32_t ticks, buckx_mngr_t *bucket, fmngr_t *fmngr,
    uint32_t brk);

#endif
