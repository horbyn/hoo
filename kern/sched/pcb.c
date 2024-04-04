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
 * @param pcb     the pcb object
 * @param scur    current stack
 * @param s0      ring0 stack
 * @param tid     thread id
 * @param pdir_va virtual address of page directory table
 * @param ticks   ticks amount
 */
void
pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, void *pdir_va,
uint32_t ticks) {

    if (pcb == null)    panic("pcb_set(): parameter invalid");
    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    pcb->pdir_va_ = pdir_va;
    bzero(&pcb->vspace_, sizeof(vspace_t));
    pcb->ticks_ = ticks;
}

/**
 * @brief get tid of the pcb object
 * 
 * @param pcb pcb object
 * @return thread id
 */
tid_t
pcb_get_tid(pcb_t *pcb) {
    if (pcb == null)    panic("pcb_get_tid(): parameter invalid");
    return pcb->tid_;
}
