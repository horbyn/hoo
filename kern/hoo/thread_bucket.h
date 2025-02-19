#pragma once
#ifndef __KERN_HOO_THREAD_BUCKET_H__
#define __KERN_HOO_THREAD_BUCKET_H__

#include "kern/mem/bucket.h"
#include "kern/sched/pcb.h"

// bucket 数组，每个元素表示大小: 8, 16, 32, 64, 128, 256, 512, 1024
#define MAX_BUCKET_SIZE     8

/**
 * @brief 线程 bucket 管理器
 */
typedef struct arr_buckmngr {
    buckx_mngr_t head_[MAX_BUCKET_SIZE];
} thread_buckmngr_t;

void         init_thread_buckmngr(pcb_t *pcb);
buckx_mngr_t *thread_buckmngr_get(tid_t tid);
void         thread_buckmngr_clear(tid_t tid);

#endif
