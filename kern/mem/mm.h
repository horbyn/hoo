/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_MM_H__
#define __KERN_MEM_MM_H__

#include "kern/x86.h"
#include "kern/conf/page.h"
#include "kern/driver/io.h"
#include "kern/units/bitmap.h"
#include "kern/units/spinlock.h"

// how many bit to map 4GB if one bit maps one page
#define SIZE_BITMAP_PHYMM4G \
    ((0xffffffff / (PGSIZE) + 1) / BITS_PER_BYTE)

/**
 * @brief physical memory information
 */
typedef struct mem_info {
    uint32_t base_;
    uint32_t length_;
} mminfo_t;

void mem_info_init(void);
const mminfo_t *mem_info_get(void);
void *phy_alloc_page(void);
void phy_release_page(void *page_phy_addr);
void mapping(pgelem_t *pdir, uint32_t va, uint32_t pa);

#endif
