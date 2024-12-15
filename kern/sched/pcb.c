/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "pcb.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief setup pcb object
 * 
 * @param pcb           the pcb object
 * @param s0            ring0 stack
 * @param s3            ring3 stack
 * @param tid           thread id
 * @param pd_pa         page directory physical address
 * @param vmngr         virtual space manager
 * @param ticks         ticks amount
 * @param sleep_on_here sleep to wait for resources
 * @param bucket        bucket manager
 * @param fmngr         files manager
 * @param brk           the end
 * @param parent        the parent
 * @param curdir        the current directory
 */
void
pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
vspace_t *vmngr, uint32_t ticks, void *sleep_on_here, buckx_mngr_t *bucket,
fmngr_t *fmngr, uint32_t brk, tid_t parent, curdir_t *curdir) {
    if (pcb == null)    panic("pcb_set(): null pointer");
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");

    pcb->stack0_ = s0;
    pcb->stack3_ = s3;
    pcb->tid_ = tid;
    pcb->pgdir_pa_ = pd_pa;
    if (vmngr == null)    vspace_set(&pcb->vmngr_, null, 0, 0, null);
    else if (vmngr != &pcb->vmngr_)
        memmove(&pcb->vmngr_, vmngr, sizeof(vspace_t));
    pcb->ticks_ = ticks;
    pcb->sleep_ = sleep_on_here;
    pcb->hmngr_ = bucket;
    pcb->fmngr_ = fmngr;
    pcb->break_ = brk;
    pcb->parent_ = parent;
    pcb->curdir_ = curdir;
}
