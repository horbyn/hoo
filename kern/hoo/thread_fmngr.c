#include "thread_fmngr.h"
#include "kern/panic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"

static uint8_t *__bitmap_buff;
static fd_t    *__files_array;
static fmngr_t *__mdata_fmngr;

/**
 * @brief 初始化所有线程的文件管理器
 * 
 * @param pcb 需要分配文件管理器的 PCB
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

        // 用于 stdin, stdout, stderr
        bitmap_set(&(__mdata_fmngr + i)->fd_set_, FD_STDIN);
        bitmap_set(&(__mdata_fmngr + i)->fd_set_, FD_STDOUT);
        bitmap_set(&(__mdata_fmngr + i)->fd_set_, FD_STDERR);
    }
}

/**
 * @brief 获取线程的文件管理器
 * 
 * @param tid 线程 id
 * @return 文件管理器
 */
fmngr_t *
thread_fmngr_get(tid_t tid) {
    if (tid >= MAX_TASKS_AMOUNT)
        panic("thread_buckmngr_get(): thread id out of range");
    return __mdata_fmngr + tid;
}

/**
 * @brief 清空文件管理器
 * 
 * @param tid 线程 id
 */
void
thread_fmngr_clear(tid_t tid) {
    fmngr_init(thread_fmngr_get(tid));
}
