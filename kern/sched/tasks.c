/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"
#include "kern/module/idle.h"

// serially access thread id
static spinlock_t __spinlock_alloc_tid;
static tid_t __global_tid;

/**
 * @brief setup pcb object
 * 
 * @param pcb the pcb object
 * @param scur current stack
 * @param s0   ring0 stack
 * @param tid  thread id
 * @param pdir_va virtual address of page directory table
 */
void
pcb_set(pcb_t *pcb, uint32_t *scur, uint32_t *s0, uint32_t tid, void *pdir_va) {

    if (pcb == null)    panic("pcb_set(): parameter invalid");
    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    pcb->pdir_va_ = pdir_va;
    bzero(&pcb->vspace_, sizeof(vspace_t));
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

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    // TODO
    return __pcb_idle;
}

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    spinlock_init(&__spinlock_alloc_tid);
}

/**
 * @brief Get the thread id
 * 
 * @return id
 */
tid_t
tid_get() {
    tid_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = __global_tid++;
    signal(&__spinlock_alloc_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("tid_get(): thread id overflows");
    return temp;
}
