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
#include "kern/units/list.h"

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

void vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length);
void vspace_set(vspace_t *vs, list_t *ls, uint32_t begin, uint32_t end, vspace_t *next);
void vspace_append(vspace_t *cur, vspace_t *next);

#endif
