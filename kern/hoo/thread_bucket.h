/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_HOO_THREAD_BUCKET_H__
#define __KERN_HOO_THREAD_BUCKET_H__

#include "kern/mem/bucket.h"
#include "kern/mem/vm.h"
#include "kern/sched/pcb.h"

// bucket size array: 8, 16, 32, 64, 128, 256, 512, 1024
#define MAX_BUCKET_SIZE     8

/**
 * @brief the bucket manager one thread owned
 */
typedef struct arr_buckmngr {
    buckx_mngr_t head_[MAX_BUCKET_SIZE];
} thread_buckmngr_t;

void         init_thread_buckmngr(pcb_t *pcb);
buckx_mngr_t *thread_buckmngr_get(tid_t tid);

#endif
