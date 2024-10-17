/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_PM_H__
#define __KERN_MEM_PM_H__

#include "kern/x86.h"
#include "kern/page/page_stuff.h"
#include "kern/utilities/bitmap.h"

// how many bit to map 4GB if one bit maps one page
#define SIZE_BITMAP_PHYMM4G \
    ((0xffffffff / (PGSIZE) + 1) / BITS_PER_BYTE)

void init_phymm_system(uint32_t mem_size);
void *phy_alloc_page(void);
void phy_release_page(void *page_phy_addr);
void set_mapping(void *va, void *pa, pgelem_t flags);

#endif
