/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "pcb.h"

/**
 * @brief setup pcb object
 * 
 * @param pcb      the pcb object
 * @param scur     current stack
 * @param s0        ring0 stack
 * @param tid       thread id
 * @param pgs       pgstruct object
 * @param va_vspace virtual address of vspace metadata
 * @param va_node   virtual address of node metadata
 * @param va_vaddr  virtual address of vaddr metadata
 * @param vspace    virtual space object
 * @param ticks     ticks amount
 * @param sleeplock sleeplock
 * @param bucket    bucket manager
 * @param fmngr     files manager
 */
void
pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, pgstruct_t *pgs,
void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace, uint32_t ticks,
sleeplock_t *sleeplock, buckx_mngr_t *bucket, fmngr_t *fmngr) {
    if (pcb == null)    panic("pcb_set(): null pointer");
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");

    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    memmove(&pcb->pgstruct_, pgs, sizeof(pgstruct_t));
    if (vspace == null)    bzero(&pcb->vmngr_.head_, sizeof(vspace_t));
    else    memmove(&pcb->vmngr_.head_, vspace, sizeof(vspace_t));
    vsmngr_set(&pcb->vmngr_, va_vspace, va_node, va_vaddr);
    pcb->ticks_ = ticks;
    pcb->sleeplock_ = sleeplock;
    pcb->hmngr_ = bucket;
    pcb->fmngr_ = fmngr;
}
