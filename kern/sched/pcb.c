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
 */
void
pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace, uint32_t ticks,
sleeplock_t *sleeplock, buckx_mngr_t *bucket, fmngr_t *fmngr, uint32_t brk) {
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
    pcb->fmngr_ = fmngr;
    pcb->break_ = brk;
}

#ifdef DEBUG
void
debug_print_pcb(pcb_t *pcb) {
    kprintf("[DEBUG] .stack0(%dB)=%p, .stack3(%dB)=%p, .tid(%dB)=%d, "
        ".pgdir pa(%dB)=%p, .vmngr.vspace=%p, .vmngr.node=%p, .vmngr.vaddr=%p, "
        ".vmngr.head=%p, ",
        sizeof(uint32_t *), pcb->stack0_, sizeof(uint32_t *), pcb->stack3_,
        sizeof(uint32_t *), pcb->tid_, sizeof(pgelem_t *), pcb->pgdir_pa_,
        pcb->vmngr_.vspace_, pcb->vmngr_.node_, pcb->vmngr_.vaddr_, pcb->vmngr_.head_);
    kprintf(".ticks(%dB)=%d, .sleeplock(%dB)=%p, .hmngr(%dB)=%p, "
        ".fmngr(%dB)=%p, .break(%dB)=%d\n",
        sizeof(uint32_t), pcb->ticks_, sizeof(sleeplock_t *), pcb->sleeplock_,
        sizeof(buckx_mngr_t *), pcb->hmngr_, sizeof(fmngr_t *), pcb->fmngr_,
        sizeof(uint32_t), pcb->break_);
}
#endif
