/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_SUPER_BLOCK_H__
#define __KERN_FS_SUPER_BLOCK_H__

#include "fs_stuff.h"
#include "kern/driver/ata_driver.h"
#include "kern/debug.h"
#include "kern/lib/lib.h"

/**
 * @brief super block
 */
typedef struct super_block {
    uint32_t magic_;
    lba_index_t lba_partition_,
        lba_super_block_,
        lba_map_inode_,
        lba_inodes_,
        lba_map_free_,
        lba_free_;
    enum_index_level_t index_level_;
} super_block_t;

bool setup_super_block(super_block_t *, uint32_t, enum_index_level_t);

#endif
