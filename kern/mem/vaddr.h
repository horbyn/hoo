/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEM_ADDR_MANAGER_H__
#define __KERN_MEM_ADDR_MANAGER_H__

#include "conf/Page.h"
#include "kern/lib/lib.h"
#include "kern/lib/list.h"

/**
 * @brief interval
 */
typedef struct interval {
    uint32_t begin_;
    uint32_t end_;
} __attribute__((packed)) interval_t;

/**
 * @brief virtual address
 */
typedef struct vaddr_list {
    list_t            *list_;
    interval_t        inte_;
    struct vaddr_list *next_;
} __attribute__((packed)) vaddr_list_t;

void vaddr_init(vaddr_list_t *space);
void *vaddr_alloc(vaddr_list_t *space, uint32_t size);
bool vaddr_free(vaddr_list_t *space, void *vaddr);

#endif
