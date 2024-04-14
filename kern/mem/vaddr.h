/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VADDR_H__
#define __KERN_MEM_VADDR_H__

#include "kern/x86.h"
#include "kern/units/lib.h"
#include "kern/units/format_list.h"

/**
 * @brief virtual address
 */
typedef struct vaddr {
    uint32_t va_;
    // the page amounts after the va
    uint32_t length_;
} __attribute__((packed)) vaddr_t;

/**
 * @brief virtual address list
 */
typedef struct vaddr_space {
    list_t             list_;
    uint32_t           begin_;
    uint32_t           end_;
    struct vaddr_space *next_;
} __attribute__((packed)) vspace_t;

/**
 * @brief virtual space manager
 */
typedef struct vspace_manager {
    // the metadata
    fmtlist_t *vspace_, *node_, *vaddr_;
    // the real data
    vspace_t  head_;
} __attribute__((packed)) vsmngr_t;

void vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length);
void vspace_set(vspace_t *vs, list_t *ls, uint32_t begin, uint32_t end, vspace_t *next);
void vspace_append(vspace_t *cur, vspace_t *next);
void vsmngr_init(vsmngr_t *mngr, void *va_vs, void *va_node, void *va_vaddr);
vspace_t *vsmngr_alloc_vspace(vsmngr_t *mngr);
node_t   *vsmngr_alloc_node(vsmngr_t *mngr);
vaddr_t  *vsmngr_alloc_vaddr(vsmngr_t *mngr);
void vsmngr_release_vspace(vsmngr_t *mngr, vspace_t *vs);
void vsmngr_release_node(vsmngr_t *mngr, node_t *node);
void vsmngr_release_vaddr(vsmngr_t *mngr, vaddr_t *vaddr);

#endif
