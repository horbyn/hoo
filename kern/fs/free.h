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
#include "kern/mem/dynamic.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/units/bitmap.h"

lba_index_t free_allocate();
void free_release(lba_index_t index);
void free_rw_disk(void *buff, lba_index_t base_lba, ata_cmd_t cmd, bool is_new);
void setup_free_map(bool is_new);

#endif