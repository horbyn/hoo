/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VM_KERN_H__
#define __KERN_MEM_VM_KERN_H__

#include "kern/utilities/bitmap.h"

#define KERN_METADATA       0xf0000000
// the maximum value of hoo virtual space -- the cause that subtracted from
// 4-MB is the last entry of its page directory table is not allowed to use
#define SIZE_BITMAP_VIRMM \
    ((PG_MASK - MB4 - KERN_METADATA) / PGSIZE / BITS_PER_BYTE)

void init_kern_virmm_bitmap(void);
void *vir_alloc_kern(void);
void vir_release_kern(void *va);

#endif
