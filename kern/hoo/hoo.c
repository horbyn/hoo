#include "hoo.h"
#include "thread_bucket.h"
#include "thread_curdir.h"
#include "thread_fmngr.h"
#include "thread_pcb.h"
#include "thread_tid.h"
#include "kern/fs/files.h"
#include "kern/module/log.h"

/**
 * @brief 获取 hoo 的页目录表
 * 
 * @return 页目录表
 */
pgelem_t *
get_hoo_pgdir(void) {
    return (pgelem_t *)(SEG_PDTABLE * 16 + KERN_HIGH_MAPPING);
}

/**
 * @brief 获取 hoo 的 TSS
 * @note TSS 整个系统唯一
 * @return TSS 对象
 */
tss_t *
get_hoo_tss(void) {
    static tss_t tss;
    return &tss;
}

/**
 * @brief 获取 hoo 的 PCB
 * 
 * @return PCB 对象
 */
pcb_t *
get_hoo_pcb(void) {
    // hoo 使用静态分配的 PCB 而其他进程则动态分配
    static pcb_t hoo_pcb;
    return &hoo_pcb;
}

/**
 * @brief 获取 hoo 的 cache buffer
 * 
 * @return cache buffer 对象
 */
cachebuff_t *
get_hoo_cache_buff(void) {
    static cachebuff_t hoo_log_cache;
    static bool is_init = false;
    if (is_init == false) {
        cachebuff_set(&hoo_log_cache, (char *)STACK_HOO_RING0, PGSIZE * 2);
        is_init = true;
    }
    return &hoo_log_cache;
}

/**
 * @brief 初始化 hoo 进程
 */
void
init_hoo(void) {

    pcb_t *hoo_pcb = get_hoo_pcb();

    /*
     * 内核线性地址空间如下：
     * kernel space：所有普通进程共享
     * kernel dynamic spcae：内核自己使用的可动态分配的空间
     * kernel metadata space：内核的管理数据
     *
     *                            kernel space    dynamic space
     * +----------+---------------+---------------+-------------------------+
     * |kernel elf|               |elf mapping    | dynamic   ..  metadata  |
     * +----------+---------------+---------------+-------------------------+
     *            0x00xx_x000     0x8000_0000     0x80xx_x000     0xf000_0000
     */

    // 动态内存空间开始于 0x0040_0000（对于普通进程来说是 0x8040_0000），
    //     因为内核非常小，小于一页（4MB），前面 0x40_0000 的空间够了
    hoo_pcb->break_ = KERN_HIGH_MAPPING + MB4;
    vspace_set(&hoo_pcb->vmngr_, null, 0, 0, null);

    // initialize some metadata
    init_thread_buckmngr(hoo_pcb);
    init_thread_curdir(hoo_pcb);
    init_thread_fmngr(hoo_pcb);
    init_thread_pcb(hoo_pcb);
    init_thread_tid(hoo_pcb);

    // 从引导阶段到现在，一直有个执行流，现在将这个控制流作为 hoo 进程，
    //    将 boot 阶段使用着的栈作为 hoo 的 ring0 栈
    pcb_set(hoo_pcb, (uint32_t *)STACK_HOO_RING0, (uint32_t *)STACK_HOO_RING3,
        TID_HOO, (pgelem_t *)(V2P(get_hoo_pgdir())), &hoo_pcb->vmngr_, TIMETICKS,
        null, thread_buckmngr_get(TID_HOO), thread_fmngr_get(TID_HOO),
        hoo_pcb->break_, INVALID_INDEX, thread_curdir_get(TID_HOO));

}
