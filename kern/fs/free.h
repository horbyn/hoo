/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_FREE_H__
#define __KERN_FS_FREE_H__

#include "super_block.h"
#include "kern/dyn/dynamic.h"
#include "kern/utilities/bitmap.h"

void     setup_free_map(bool is_new);
uint32_t free_allocate(void);
void     free_map_setup(uint32_t index, bool is_set);
void     free_map_update(void);
void     free_rw_disk(void *buff, uint32_t base_lba, atacmd_t cmd);

#endif
