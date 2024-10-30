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
 * @param vmngr     virtual space manager
 * @param ticks     ticks amount
 * @param bucket    bucket manager
 * @param fmngr     files manager
 * @param brk       the end
 */
void
pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
vspace_t *vmngr, uint32_t ticks, buckx_mngr_t *bucket, fmngr_t *fmngr, uint32_t brk) {
    if (pcb == null)    panic("pcb_set(): null pointer");
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");

    pcb->stack0_ = s0;
    pcb->stack3_ = s3;
    pcb->tid_ = tid;
    pcb->pgdir_pa_ = pd_pa;
    if (vmngr == null)    vspace_set(&pcb->vmngr_, null, 0, 0, null);
    else    memmove(&pcb->vmngr_, vmngr, sizeof(vspace_t));
    pcb->ticks_ = ticks;
    pcb->hmngr_ = bucket;
    pcb->fmngr_ = fmngr;
    pcb->break_ = brk;
}
