/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "tasks.h"

/**
 * @brief bucket size array
 */
static uint32_t __bucket_size[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };
#define MAX_BUCKET_SIZE     (NELEMS(__bucket_size))

/**
 * @brief the bucket manager one thread owned
 */
typedef struct arr_buckmngr {
    buckx_mngr_t head_[MAX_BUCKET_SIZE];
} thread_buckmngr_t;

static queue_t __queue_ready, __queue_running;
static thread_buckmngr_t *__mdata_buckmngr;

/**
 * @brief get the bucket manager metadata
 * 
 * @param tid thread id
 * @return bucket manager metadata
 */
static buckx_mngr_t *
thread_buckmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_buckmngr[tid].head_;
}

/**
 * @brief bucket manager initialization of single thread
 * 
 * @param tb thread bucket manager
 */
static void
thread_buckmngr_set(thread_buckmngr_t *tb) {
    if (tb == null)    panic("thread_buckmngr_set(): null pointer");

    buckx_mngr_t *worker = tb->head_;
    for (int i = 0; i < MAX_BUCKET_SIZE; ++i) {
        buckx_mngr_t *next = (i == MAX_BUCKET_SIZE - 1) ? null : worker + i + 1;
        buckmngr_init(worker + i, __bucket_size[i], null, next);
    }
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
 * @brief initialize the tasks system
 */
void
init_tasks_system() {
    queue_init(&__queue_ready);
    queue_init(&__queue_running);
    __mdata_buckmngr = null;

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_t *hoo_pcb = get_hoo_pcb();
    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;
    uint32_t hoo_break = PGUP(null, PGSIZE);

    // allocate metadata
    uint32_t metadata_bucket_pages =
        (sizeof(thread_buckmngr_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_buckmngr =
        vir_alloc_pages(&hoo_pcb->vmngr_, metadata_bucket_pages, hoo_break);
    for (uint32_t i = 0; i < metadata_bucket_pages; ++i) {
        void *va = (void *)((uint32_t)__mdata_buckmngr + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }
    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i)
        thread_buckmngr_set(__mdata_buckmngr + i);

    // setup hoo pcb
    pcb_set(hoo_pcb, (uint32_t *)STACK_HOO_RING0, (uint32_t *)STACK_HOO_RING3,
        TID_HOO, (pgelem_t *)(V2P(get_hoo_pgdir())), null, TIMETICKS,
        thread_buckmngr_get(TID_HOO), hoo_break);

    // pcb node append to queue
    static node_t hoo_node;
    node_set(&hoo_node, hoo_pcb, null);
    queue_push(&__queue_running, &hoo_node, TAIL);
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
