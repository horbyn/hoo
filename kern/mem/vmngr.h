/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VADDR_H__
#define __KERN_MEM_VADDR_H__

#include "kern/utilities/format_list.h"

/**
 * @brief virtual address
 */
typedef struct vaddr {
    uint32_t va_;
    // the page amounts after the va
    uint32_t length_;
} vaddr_t;

/**
 * @brief virtual address list
 */
typedef struct vaddr_space {
    list_t             list_;
    uint32_t           begin_;
    uint32_t           end_;
    struct vaddr_space *next_;
} vspace_t;

/**
 * @brief virtual space manager
 */
typedef struct vspace_manager {
    // the metadata
    fmtlist_t *vspace_, *node_, *vaddr_;
    // the real data
    vspace_t  head_;
} vsmngr_t;

void vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length);
void vspace_set(vspace_t *vs, list_t *ls, uint32_t begin, uint32_t end, vspace_t *next);
void vsmngr_set(vsmngr_t *mngr, void *va_vs, void *va_node, void *va_vaddr);

#endif
