/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "thread_fmngr.h"

static uint8_t *__bitmap_buff;
static fd_t    *__files_array;
static fmngr_t *__mdata_fmngr;

/**
 * @brief initialize the files manager of all threads
 * 
 * @param pcb the pcb needed to allocate files manager
 */
void
init_thread_fmngr(pcb_t *pcb) {
    if (pcb == null)    panic("init_thread_fmngr(): null pointer");
    pgelem_t flag = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    uint32_t bitmap_pages =
        ((MAX_FILES_PER_TASK / BITS_PER_BYTE) * MAX_TASKS_AMOUNT + PGSIZE - 1)
        / PGSIZE;
    __bitmap_buff = vir_alloc_pages(&pcb->vmngr_, bitmap_pages, pcb->break_);
    for (uint32_t i = 0; i < bitmap_pages; ++i) {
        void *va = (void *)__bitmap_buff + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flag);
    }

    uint32_t array_pages =
        (sizeof(fd_t) * MAX_FILES_PER_TASK * MAX_TASKS_AMOUNT + PGSIZE - 1)
        / PGSIZE;
    __files_array = vir_alloc_pages(&pcb->vmngr_, array_pages, pcb->break_);
    for (uint32_t i = 0; i < array_pages; ++i) {
        void *va = (void *)__files_array + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flag);
    }

    uint32_t fmngr_pages =
        (sizeof(fmngr_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_fmngr = vir_alloc_pages(&pcb->vmngr_, fmngr_pages, pcb->break_);
    for (uint32_t i = 0; i < fmngr_pages; ++i) {
        void *va = (void *)__mdata_fmngr + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flag);
    }

    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i) {
        bitmap_init(&((__mdata_fmngr + i)->fd_set_), MAX_FILES_PER_TASK,
            __bitmap_buff + (i * (MAX_FILES_PER_TASK / BITS_PER_BYTE)));
        (__mdata_fmngr + i)->files_ = __files_array + (i * MAX_FILES_PER_TASK);
    }
}

/**
 * @brief get the files manager metadata
 * 
 * @param tid thread id
 * @return files manager metadata
 */
fmngr_t *
thread_fmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_fmngr + tid;
}
