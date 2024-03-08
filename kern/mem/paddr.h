/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEM_PADDR_H__
#define __KERN_MEM_PADDR_H__

#include "page.h"
#include "kern/types.h"
#include "kern/lib/bitmap.h"

#define SIZE_BITMAP_PHYMM4G \
    ((0xffffffff / (PGSIZE) + 1) / BITS_PER_BYTE)           // how many bit to map 4GB if one bit maps one page

void *phy_alloc_page();
void phy_release_page(void *page_phy_addr);

#endif
