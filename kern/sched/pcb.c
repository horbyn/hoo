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
 * @param pcb       the pcb object
 * @param s0        ring0 stack
 * @param s3        ring3 stack
 * @param tid       thread id
 * @param pd_pa     page directory physical address
 * @param va_vspace virtual address of vspace metadata
 * @param va_node   virtual address of node metadata
 * @param va_vaddr  virtual address of vaddr metadata
 * @param vspace    virtual space object
 * @param ticks     ticks amount
 * @param sleeplock sleeplock
 * @param bucket    bucket manager
 * @param fmngr     files manager
 * @param break     the end
 * @param parent    the parent
 * @param dir       the current directory
 * @param dirlen    the length of current directory
 */
void
pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace, uint32_t ticks,
sleeplock_t *sleeplock, buckx_mngr_t *bucket, fmngr_t *fmngr, uint32_t brk,
tid_t parent, char *dir, uint32_t dirlen) {
    if (pcb == null)    panic("pcb_set(): null pointer");
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");

    pcb->stack0_ = s0;
    pcb->stack3_ = s3;
    pcb->tid_ = tid;
    pcb->pgdir_pa_ = pd_pa;
    if (vspace == null)    bzero(&pcb->vmngr_.head_, sizeof(vspace_t));
    else    memmove(&pcb->vmngr_.head_, vspace, sizeof(vspace_t));
    vsmngr_set(&pcb->vmngr_, va_vspace, va_node, va_vaddr);
    pcb->ticks_ = ticks;
    pcb->sleeplock_ = sleeplock;
    pcb->hmngr_ = bucket;
    if (fmngr == null)    bzero(&pcb->fmngr_, sizeof(fmngr_t));
    else    memmove(&pcb->fmngr_, fmngr, sizeof(fmngr_t));
    pcb->break_ = brk;
    pcb->parent_ = parent;
    pcb->dir_ = dir;
    pcb->dirlen_ = dirlen;
}
