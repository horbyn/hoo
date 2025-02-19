#pragma once
#ifndef __KERN_MEM_VM_KERN_H__
#define __KERN_MEM_VM_KERN_H__

#include "kern/x86.h"
#include "kern/page/page_stuff.h"
#include "kern/utilities/bitmap.h"

#define KERN_METADATA       0xf0000000
// hoo 线性地址空间的最大寻址范围，需要减去 4MB，是因为最后一个页表不能被使用
#define SIZE_BITMAP_VIRMM \
    ((PG_MASK - MB4 - KERN_METADATA) / PGSIZE / BITS_PER_BYTE)

void init_kern_virmm_bitmap(void);
void *vir_alloc_kern(void);
void vir_release_kern(void *va);

#endif
