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
 * @param va_pdir virtual address of page directory table
 * @param ticks   ticks amount
 */
void
pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, void *va_pdir,
void *va_vspace, void *va_node, void *va_vaddr, uint32_t ticks) {

    if (pcb == null)    panic("pcb_set(): parameter invalid");
    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    pcb->pdir_va_ = va_pdir;
    bzero(&pcb->vmngr_, sizeof(vsmngr_t));
    vsmngr_init(&pcb->vmngr_, va_vspace, va_node, va_vaddr);
    pcb->ticks_ = ticks;
}
