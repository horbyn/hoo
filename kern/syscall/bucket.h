/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SYSCALL_BUCKET_T__
#define __KERN_SYSCALL_BUCKET_T__

#include "kern/mem/paddr.h"
#include "kern/types.h"

/**
 * @brief memory bucket
 */
typedef struct bucket {
    void *head_;                                            // the space to be allocated
    uint32_t ref_;                                          // reference count for release
} bucket_t;

/**
 * @brief memory bucket descriptor
 */
typedef struct bucket_descriptor {
    uint32_t capacity_;
    bucket_t *buckets_;
} bucket_desc_t;

bucket_t *bucket_init();

#endif
