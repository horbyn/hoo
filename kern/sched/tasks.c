/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

// serially access thread id
static spinlock_t __spinlock_alloc_tid;
// serially access task queues
static spinlock_t __spinlock_tasks_queue;
static queue_t __queue_ready, __queue_running;
static tid_t __global_tid;

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    wait(&__spinlock_tasks_queue);
    node_t *cur = queue_front(&__queue_running);
    signal(&__spinlock_tasks_queue);
    if (!cur)
        panic("get_current_pcb(): no current task");

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    if (!cur_pcb)
        panic("get_current_pcb(): current task is null");

    return cur_pcb;
}

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    spinlock_init(&__spinlock_alloc_tid);
    spinlock_init(&__spinlock_tasks_queue);
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

/**
 * @brief scheduler two thread
 */
void
scheduler() {
    if (test(&__spinlock_tasks_queue))    return;

    // to check whether ticks expired
    // we just need the first queue node
    wait(&__spinlock_tasks_queue);
    node_t *cur = queue_front(&__queue_running);
    if (cur) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            signal(&__spinlock_tasks_queue);
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
        tss_t *tss = get_idle_tss();
        tss->ss0_ = DS_SELECTOR_KERN;
        tss->esp0_ = (uint32_t)((pcb_t *)next->data_)->stack0_;

        signal(&__spinlock_tasks_queue);
        switch_to(cur, next);
    }

    signal(&__spinlock_tasks_queue);
}

/**
 * @brief make a task get ready
 * 
 * @param task the node of a task pcb object
 */
void
task_ready(node_t *task) {
    wait(&__spinlock_tasks_queue);
    queue_push(&__queue_ready, task, TAIL);
    signal(&__spinlock_tasks_queue);
}
