#pragma once
#ifndef __KERN_MEM_PM_H__
#define __KERN_MEM_PM_H__

#include "kern/x86.h"
#include "kern/page/page_stuff.h"
#include "kern/utilities/bitmap.h"

// 如果一个比特位映射一个物理页，那么有多少个比特位可以映射完整的 4GB
#define SIZE_BITMAP_PHYMM4G \
    ((0xffffffff / (PGSIZE) + 1) / BITS_PER_BYTE)

void init_phymm_system(uint32_t mem_size);
void *phy_alloc_page(void);
void phy_release_page(void *phy_addr);
void phy_release_vpage(void *vir_addr);
void set_mapping(void *va, void *pa, pgelem_t flags);

#endif
