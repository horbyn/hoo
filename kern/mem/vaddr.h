/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEM_ADDR_MANAGER_H__
#define __KERN_MEM_ADDR_MANAGER_H__

#include "kern/mem/page.h"
#include "kern/lib/lib.h"
#include "kern/lib/list.h"

#define MAX_SPACE   0xc0000000                              // maxinum virtual space

/**
 * @brief interval
 */
typedef struct interval {
    uint32_t begin_;
    uint32_t end_;
} __attribute__((packed)) interval_t;

/**
 * @brief virtual address list
 */
typedef struct vaddr_list {
    list_t            *list_;
    interval_t        inte_;
    struct vaddr_list *next_;
} __attribute__((packed)) vaddr_list_t;

/**
 * @brief virtual address
 */
typedef struct vaddr {
    uint32_t addr_;
    uint32_t size_;
} __attribute__((packed)) vaddr_t;

void set_vaddrlist(vaddr_list_t *dlist, list_t *llist, uint32_t begin, uint32_t end, vaddr_list_t *next);
void set_vaddr(vaddr_t *vaddr, uint32_t addr, uint32_t size);
void *vaddr_get(vaddr_list_t *space, uint32_t size);
bool vaddr_free(vaddr_list_t *space, void *vaddr);

#endif
