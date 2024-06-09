/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_BUCKET_MNGR_H__
#define __KERN_MEM_BUCKET_MNGR_H__

#include "kern/x86.h"
#include "kern/driver/io.h"

/**
 * @brief use array as linked list
 */
typedef struct array_list {
    // the size of the specific type
    uint32_t          type_size_;
    // list
    void              *head_;
    // first element offset
    uint32_t          first_off_;
    // capacity
    uint32_t          capacity_;
    // current size
    uint32_t          size_;
    struct array_list *next_;
} __attribute__((packed)) arrlist_t;

/**
 * @brief memory bucket manager
 */
typedef struct buckX_manager {
    uint32_t             size_;
    arrlist_t            *chain_;
    struct buckX_manager *next_;
} buckx_mngr_t;

void buckmngr_init(buckx_mngr_t *mngr, uint32_t size, arrlist_t *chain,
    buckx_mngr_t *next);

#endif
