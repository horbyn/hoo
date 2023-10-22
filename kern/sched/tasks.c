/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "tasks.h"

queue_t __queue_ready, __queue_running;
static size_t __global_tid;
static spinlock_t __spinlock_alloc_tid;

/**
 * @brief Get the thread id
 * 
 * @return id
 */
static size_t
get_tid() {
    size_t temp = 0;

    wait(&__spinlock_alloc_tid);
    temp = ++__global_tid;
    signal(&__spinlock_alloc_tid);

    return temp;
}

/**
 * @brief initialize the tasks queue
 */
void
init_tasks_queue() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    spinlock_init(&__spinlock_alloc_tid);
}

/**
 * @brief fill in pcb
 * 
 * @param pcb       pcb to be filled
 * @param cur_stack current stack
 * @param stack0    ring0 stack
 * @param ticks     ticks amount
 * @param tid       thread id
 */
void
pcb_fill(pcb_t *pcb, uint32_t *cur_stack, uint32_t *stack0, uint32_t ticks) {
    bzero(pcb, sizeof(pcb_t));
    pcb->stack_ = cur_stack;
    pcb->stack0_ = stack0;
    pcb->ticks_ = ticks;
    pcb->tid_ = get_tid();
}

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_pcb() {
    node_t *cur = queue_front(&__queue_running);
    ASSERT(!cur);

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    ASSERT(!cur_pcb);

    return cur_pcb;
}

/**
 * @brief scheduler two thread
 */
void
scheduler() {
    // to check whether ticks expired
    // we just need the first queue node
    node_t *cur = queue_front(&__queue_running);
    if (cur) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            return;
        } else    ((pcb_t *)cur->data_)->ticks_ = TIMETICKS;
    }

    node_t *next = queue_pop(&__queue_ready);
    if (next) {
        cur = queue_pop(&__queue_running);
        // only change tasks when the `cur` task exists
        if (cur)
            queue_push(&__queue_ready, cur, TAIL);
        queue_push(&__queue_running, next, TAIL);

        // update tss
        __tss.ss0_ = DS_SELECTOR_KERN;
        __tss.esp0_ = (uint32_t)((pcb_t *)next->data_)->stack0_;
        
        switch_to(cur, next);
    }
}

/**
 * @brief make current task to sleep
 * 
 * @param q the sleeping queue
 */
void
sleep(queue_t *q) {
    ASSERT(!queue_front(&__queue_running));

    // drop current tasks and enqueue sleep queue

    node_t *cur = queue_pop(&__queue_running);
    if (cur)    queue_push(q, cur, TAIL);
}

/**
 * @brief wakeup a thread of the specified channel
 * 
 * @param q the sleeping queue
 */
void
wakeup(queue_t *q) {
    ASSERT(!queue_front(q));

    node_t *cur = queue_pop(q);
    if (cur)    queue_push(&__queue_ready, cur, TAIL);
}
