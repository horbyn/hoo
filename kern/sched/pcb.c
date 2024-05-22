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

/**
 * @brief setup pgstruct object
 * 
 * @param pgs      pgstruct object
 * @param pgdir_va virtual address of the page directory table
 * @param pgdir_pa physical address of the page directory table
 * @param mapping  the page table mapping
 */
void
pgstruct_set(pgstruct_t *pgs, void *pgdir_va, void *pgdir_pa, pgelem_t *mapping) {
    if (pgs == null)    panic("pgstruct_set(): null pointer");

    pgs->pdir_va_ = pgdir_va;
    pgs->pdir_pa_ = pgdir_pa;
    pgs->mapping_ = mapping;
}

/**
 * @brief setup pcb object
 * 
 * @param scur     current stack
 * @param s0        ring0 stack
 * @param tid       thread id
 * @param pgs       pgstruct object
 * @param va_vspace virtual address of vspace metadata
 * @param va_node   virtual address of node metadata
 * @param va_vaddr  virtual address of vaddr metadata
 * @param vspace    virtual space object
 * @param ticks     ticks amount
 * @param sleeplock sleeplock
 */
void
pcb_set(uint32_t *scur, uint32_t *s0, uint32_t tid, pgstruct_t *pgs,
void *va_vspace, void *va_node, void *va_vaddr, vspace_t *vspace, uint32_t ticks,
sleeplock_t *sleeplock) {

    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");
    pcb_t *pcb = pcb_get(tid);
    pcb->stack_cur_ = scur;
    pcb->stack0_ = s0;
    pcb->tid_ = tid;
    memmove(&pcb->pgstruct_, pgs, sizeof(pgstruct_t));
    if (vspace == null)    bzero(&pcb->vmngr_.head_, sizeof(vspace_t));
    else    memmove(&pcb->vmngr_.head_, vspace, sizeof(vspace_t));
    vsmngr_init(&pcb->vmngr_, va_vspace, va_node, va_vaddr);
    pcb->ticks_ = ticks;
    pcb->sleeplock_ = sleeplock;
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
}

/**
 * @brief get the bitmap storing thread id
 * 
 * @return bitmap object
 */
static bitmap_t *
get_bitmap_tid(void) {
    __attribute__((aligned(16)))
        static uint8_t bmbuff_tid[MAX_TASKS_AMOUNT / BITS_PER_BYTE];
    static bitmap_t bm_tid;
    static bool is_init = false;
    if (!is_init) {
        bzero(bmbuff_tid, sizeof(bmbuff_tid));
        bitmap_init(&bm_tid, MAX_TASKS_AMOUNT, bmbuff_tid);
        // for hoo thread
        bitmap_set(&bm_tid, TID_HOO);
        // for idle thread
        bitmap_set(&bm_tid, TID_IDLE);
        is_init = true;
    }
    return &bm_tid;
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
    temp = (tid_t)bitmap_scan_empty(get_bitmap_tid());
    signal(&__spinlock_alloc_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("allocate_tid(): thread id overflows");
    return temp;
}
