#include "thread_tid.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/utilities/bitmap.h"
#include "kern/utilities/spinlock.h"
#include "user/lib.h"

static uint8_t *__bm_buff;
static bitmap_t __bm_tid;
static spinlock_t __sl_tid;

/**
 * @brief 初始化所有线程的线程 id
 * 
 * @param pcb 对应的 PCB
 */
void
init_thread_tid(pcb_t *pcb) {
    if (pcb == null)    panic("init_thread_tid(): null pointer");
    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    uint32_t metadata_tid_pages =
        (sizeof(uint8_t) * (MAX_TASKS_AMOUNT / BITS_PER_BYTE) + PGSIZE - 1) / PGSIZE;
    __bm_buff = vir_alloc_pages(&pcb->vmngr_, metadata_tid_pages, pcb->break_);
    for (uint32_t i = 0; i < metadata_tid_pages; ++i) {
        void *va = (void *)((uint32_t)__bm_buff + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }
    bzero(__bm_buff, sizeof(__bm_buff));
    bitmap_init(&__bm_tid, MAX_TASKS_AMOUNT, __bm_buff);

    // 用于 hoo
    bitmap_set(&__bm_tid, TID_HOO);

    spinlock_init(&__sl_tid);
}

/**
 * @brief 分配线程 id
 * 
 * @return 线程 id
 */
tid_t
thread_tid_alloc(void) {
    tid_t temp = 0;

    wait(&__sl_tid);
    temp = (tid_t)bitmap_scan_empty(&__bm_tid);
    bitmap_set(&__bm_tid, temp);
    signal(&__sl_tid);

    if (temp >= MAX_TASKS_AMOUNT)
        panic("thread_tid_alloc(): thread id overflows");
    return temp;
}

/**
 * @brief 释放线程 id
 * 
 * @param tid 线程 id
 */
void
thread_tid_release(tid_t tid) {
    wait(&__sl_tid);
    bitmap_clear(&__bm_tid, tid);
    signal(&__sl_tid);
}
