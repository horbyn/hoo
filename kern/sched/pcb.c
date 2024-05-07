/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "pcb.h"

// serially access pcb array
static spinlock_t __spinlock_pcb;
// serially access thread id
static spinlock_t __spinlock_alloc_tid;
static tid_t __global_tid;

/**
 * @brief setup pcb object
 * 
 * @param scur    current stack
 * @param s0      ring0 stack
 * @param tid     thread id
 * @param va_pdir virtual address of page directory table
 * @param pa_pdir physical address of page directory table
 * @param va_vspace virtual address of vspace metadata
 * @param va_node virtual address of node metadata
 * @param va_vaddr virtual address of vaddr metadata
 * @param vspace virtual space object
 * @param ticks   ticks amount
 */
void
pcb_set(uint32_t *scur, uint32_t *s0, uint32_t tid, void *va_pdir, void *pa_pdir,
void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace, uint32_t ticks) {

    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");
    pcb_t *pcb = pcb_get(tid);
    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    pcb->pdir_va_ = va_pdir;
    pcb->pdir_pa_ = pa_pdir;
    if (vspace == null)    bzero(&pcb->vmngr_.head_, sizeof(vspace_t));
    else    memmove(&pcb->vmngr_.head_, vspace, sizeof(vspace_t));
    vsmngr_init(&pcb->vmngr_, va_vspace, va_node, va_vaddr);
    pcb->ticks_ = ticks;
}

/**
 * @brief get pcb
 * 
 * @param tid thread id
 * @return pcb pointer
 */
pcb_t *
pcb_get(tid_t tid) {
    __attribute__((aligned(16))) static pcb_t
        pcb_slot[MAX_TASKS_AMOUNT] = { 0 };
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_get(): thread id overflow");

    pcb_t *temp = null;
    wait(&__spinlock_pcb);
    temp = &pcb_slot[tid];
    signal(&__spinlock_pcb);
    return temp;
}

/**
 * @brief pcb system initialization
 */
void
init_pcb_system(void) {
    spinlock_init(&__spinlock_pcb);
    spinlock_init(&__spinlock_alloc_tid);
    __global_tid = TID_IDLE + 1;
}

/**
 * @brief allocate the thread id
 * 
 * @return thread id
 */
tid_t
allocate_tid() {
    tid_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = __global_tid++;
    signal(&__spinlock_alloc_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("allocate_tid(): thread id overflows");
    return temp;
}
