/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_BLOCKS_H__
#define __KERN_FS_BLOCKS_H__

#include "super_block.h"
#include "kern/lib/bitmap.h"

lba_index_t block_allocate();
void block_release(lba_index_t index);
void blocks_rw_disk(void *buff, size_t bufflen, lba_index_t base_lba, ata_cmd_t cmd);
void setup_blocks_map(bool is_new);

#endif
