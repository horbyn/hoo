#include "thread_bucket.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"

static thread_buckmngr_t *__mdata_buckmngr;

/**
 * @brief 初始化线程的 bucket 管理器
 * 
 * @param buckx bucket 管理器
 */
static void
thread_buckmngr_init(buckx_mngr_t *buckx) {
    for (int i = 0; i < MAX_BUCKET_SIZE; ++i) {
        buckx_mngr_t *next = (i == MAX_BUCKET_SIZE - 1) ? null : buckx + i + 1;
        buckmngr_init(buckx + i, (1 << (i + 3)), null, next);
    }
}

/**
 * @brief 初始化所有线程的 bucket 管理器
 * 
 * @param pcb 需要分配 bucket 管理器的 PCB
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

    for (uint32_t i = 0; i < MAX_TASKS_AMOUNT; ++i)
        thread_buckmngr_init((__mdata_buckmngr + i)->head_);
}

/**
 * @brief 获取 bucket 管理器
 * 
 * @param tid 线程 id
 * @return bucket 管理器对象
 */
buckx_mngr_t *
thread_buckmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_buckmngr[tid].head_;
}

/**
 * @brief 清空 bucket 管理器
 * 
 * @param tid 线程 id
 */
void
thread_buckmngr_clear(tid_t tid) {
    thread_buckmngr_init(thread_buckmngr_get(tid));
}
