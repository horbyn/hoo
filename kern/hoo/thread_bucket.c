/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "thread_bucket.h"

static thread_buckmngr_t *__mdata_buckmngr;

/**
 * @brief initialize the bucket manager of all threads
 * 
 * @param pcb the pcb needed to allocate buckets manager
 */
void
init_thread_buckmngr(pcb_t *pcb) {
    if (pcb == null)    panic("init_thread_buckmngr(): null pointer");

    uint32_t metadata_bucket_pages =
        (sizeof(thread_buckmngr_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_buckmngr =
        vir_alloc_pages(&pcb->vmngr_, metadata_bucket_pages, pcb->break_);

    for (uint32_t i = 0; i < metadata_bucket_pages; ++i) {
        void *va = (void *)__mdata_buckmngr + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, PGFLAG_US | PGFLAG_RW | PGFLAG_PS);
    }

    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i) {
        buckx_mngr_t *worker = (__mdata_buckmngr + i)->head_;
        for (int i = 0; i < MAX_BUCKET_SIZE; ++i) {
            buckx_mngr_t *next = (i == MAX_BUCKET_SIZE - 1) ? null : worker + i + 1;
            buckmngr_init(worker + i, (1 << (i + 3)), null, next);
        }
    }
}

/**
 * @brief get the bucket manager metadata
 * 
 * @param tid thread id
 * @return bucket manager metadata
 */
buckx_mngr_t *
thread_buckmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_buckmngr[tid].head_;
}
