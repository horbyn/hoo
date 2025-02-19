#include "pcb.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief 填充 PCB
 * 
 * @param pcb           PCB 对象
 * @param s0            ring0 栈
 * @param s3            ring3 栈
 * @param tid           线程 id
 * @param pd_pa         页目录表物理地址
 * @param vmngr         vspace 对象
 * @param ticks         时间片数量
 * @param sleep_on_here 睡眠以等待资源
 * @param bucket        bucket 管理器
 * @param fmngr         文件管理器
 * @param brk           二进制数据边界
 * @param parent        父进程 id
 * @param curdir        curdir 对象
 */
void
pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
vspace_t *vmngr, uint32_t ticks, void *sleep_on_here, buckx_mngr_t *bucket,
fmngr_t *fmngr, uint32_t brk, tid_t parent, curdir_t *curdir) {
    if (pcb == null)    panic("pcb_set(): null pointer");
    if (tid >= MAX_TASKS_AMOUNT)    panic("pcb_set(): thread id overflow");

    pcb->stack0_ = s0;
    pcb->stack3_ = s3;
    pcb->tid_ = tid;
    pcb->pgdir_pa_ = pd_pa;
    if (vmngr == null)    vspace_set(&pcb->vmngr_, null, 0, 0, null);
    else if (vmngr != &pcb->vmngr_)
        memmove(&pcb->vmngr_, vmngr, sizeof(vspace_t));
    pcb->ticks_ = ticks;
    pcb->sleep_ = sleep_on_here;
    pcb->hmngr_ = bucket;
    pcb->fmngr_ = fmngr;
    pcb->break_ = brk;
    pcb->parent_ = parent;
    pcb->curdir_ = curdir;
}
