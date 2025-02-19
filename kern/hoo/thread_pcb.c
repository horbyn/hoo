#include "thread_pcb.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/utilities/spinlock.h"

static pcb_t *__mdata_pcb;
static spinlock_t __sl_pcb;

/**
 * @brief 初始化所有线程的 PCB
 * 
 * @param pcb 对应的 PCB
 */
void
init_thread_pcb(pcb_t *pcb) {
    if (pcb == null)    panic("init_thread_pcb(): null pointer");
    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    uint32_t metadata_pcb_pages =
        (sizeof(pcb_t) * MAX_TASKS_AMOUNT + PGSIZE - 1) / PGSIZE;
    __mdata_pcb = vir_alloc_pages(&pcb->vmngr_, metadata_pcb_pages, pcb->break_);
    for (uint32_t i = 0; i < metadata_pcb_pages; ++i) {
        void *va = (void *)((uint32_t)__mdata_pcb + i * PGSIZE);
        void *pa = phy_alloc_page();
        set_mapping(va, pa, flags);
    }

    spinlock_init(&__sl_pcb);
}

/**
 * @brief 获取线程的 PCB
 * 
 * @param tid 线程 id
 * @return PCB 对象
 */
pcb_t *
thread_pcb_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_pcb_get(): thread id out of range");
    pcb_t *pcb = null;
    wait(&__sl_pcb);
    pcb = __mdata_pcb + tid;
    signal(&__sl_pcb);
    return pcb;
}
