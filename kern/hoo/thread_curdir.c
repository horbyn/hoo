/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "thread_curdir.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"

static char *__mdata_dir = 0;
static curdir_t *__mdata_cd = 0;

/**
 * @brief initialize the curdir object of all threads
 * 
 * @param pcb the pcb needed to allocate current directory
 */
void
init_thread_curdir(pcb_t *pcb) {
    if (pcb == null)    panic("init_thread_curdir(): null pointer");
    pgelem_t flag = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    uint32_t dir_pages =
        (MAX_CURDIR_BUFF * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_dir = vir_alloc_pages(&pcb->vmngr_, dir_pages, pcb->break_);
    for (uint32_t i = 0; i < dir_pages; ++i) {
        void *va = (void *)__mdata_dir + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flag);
    }

    uint32_t curdir_pages =
        (sizeof(curdir_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_cd = vir_alloc_pages(&pcb->vmngr_, curdir_pages, pcb->break_);
    for (uint32_t i = 0; i < curdir_pages; ++i) {
        void *va = (void *)__mdata_cd + i * PGSIZE;
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flag);
    }

    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i) {
        curdir_init(__mdata_cd + i, __mdata_dir + i * MAX_CURDIR_BUFF,
            MAX_CURDIR_BUFF);
    }

}

/**
 * @brief get the curdir object metadata
 * 
 * @param tid thread id
 * @return curdir object metadata
 */
curdir_t *
thread_curdir_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_curdir_get(): thread id out of range");
    return __mdata_cd + tid;
}

/**
 * @brief clear the current directory
 * 
 * @param tid thread id
 */
void
thread_curdir_clear(tid_t tid) {
    curdir_t *cd = thread_curdir_get(tid);
    curdir_init(cd, cd->dir_, cd->dirlen_);
}
