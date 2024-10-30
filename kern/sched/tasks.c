/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

static queue_t __queue_ready, __queue_running;

/**
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);

    // pcb node append to queue
    node_t *hoo_node = node_alloc();
    node_set(hoo_node, get_hoo_pcb(), null);
    queue_push(&__queue_running, hoo_node, TAIL);
}

/**
 * @brief Get the pcb of current thread
 * 
 * @return the pcb of current thread
 */
pcb_t *
get_current_pcb() {
    node_t *cur = queue_front(&__queue_running);
    if (cur == null)    panic("get_current_pcb(): no current task");

    pcb_t *cur_pcb = (pcb_t *)cur->data_;
    if (cur_pcb == null)    panic("get_current_pcb(): current task is null");

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
    if (cur != null) {
        if (((pcb_t *)cur->data_)->ticks_ > 0) {
            ((pcb_t *)cur->data_)->ticks_--;
            return;
        } else    ((pcb_t *)cur->data_)->ticks_ = TIMETICKS;
    }

    node_t *next = queue_pop(&__queue_ready);
    if (next != null) {
        cur = queue_pop(&__queue_running);
        // only change tasks when the `cur` task exists
        if (cur != null)    queue_push(&__queue_ready, cur, TAIL);
        queue_push(&__queue_running, next, TAIL);

        // update tss
        tss_t *tss = get_hoo_tss();
        tss->ss0_ = DS_SELECTOR_KERN;
        tss->esp0_ = PGUP(((pcb_t *)next->data_)->stack0_, PGSIZE);

        // `switch_to()` CANNOT recogize the case that current tasks is not exist
        //     because `cur` is null which is not 0
        if (cur == null)    cur = 0;
        switch_to(cur, next);
    }
}
