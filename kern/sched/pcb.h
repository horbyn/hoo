#pragma once
#ifndef __KERN_SCHED_PCB_H__
#define __KERN_SCHED_PCB_H__

#include "kern/fs/fmngr.h"
#include "kern/mem/vspace.h"
#include "kern/mem/bucket.h"
#include "kern/page/page_stuff.h"
#include "kern/utilities/curdir.h"
#include "kern/utilities/spinlock.h"
#include "user/types.h"

// 当到达这个数量的时间片时发出 IRQ 信号
#define TIMETICKS   16
#define TID_HOO     0
#define TID_IDLE    1

/**
 * @brief Process Control Block
 */
typedef struct pcb {
    // ring0 栈栈顶
    uint32_t     *stack0_;
    // ring3 栈栈顶
    uint32_t     *stack3_;
    // 线程 id
    tid_t        tid_;
    pgelem_t     *pgdir_pa_;
    // vspace 对象
    vspace_t     vmngr_;
    // 剩余时间片
    uint32_t     ticks_;
    // 如果非空则当前进程进入睡眠
    void         *sleep_;
    // 堆内存管理器
    buckx_mngr_t *hmngr_;
    fmngr_t      *fmngr_;
    // 二进制数据在这里截止
    uint32_t     break_;
    tid_t        parent_;
    curdir_t     *curdir_;
} pcb_t;

void pcb_set(pcb_t *pcb, uint32_t *s0, uint32_t *s3, uint32_t tid, pgelem_t *pd_pa,
    vspace_t *vmngr, uint32_t ticks, void *sleep_on_here, buckx_mngr_t *bucket,
    fmngr_t *fmngr, uint32_t brk, tid_t parent, curdir_t *curdir);

#endif
