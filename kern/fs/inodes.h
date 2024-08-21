/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_INODES_H__
#define __KERN_FS_INODES_H__

#include "super_block.h"
#include "free.h"
#include "kern/mem/dynamic.h"
#include "kern/units/bitmap.h"

// inode index where the root dir is
#define INODE_INDEX_ROOT            0

/**
 * @brief inode type
 */
typedef enum inode_type {
    INODE_TYPE_FILE = 0,
    INODE_TYPE_DIR = 1
} enum_inode_type;

/**
 * @brief definition of inode
 */
typedef struct inode {
    uint32_t    size_;
    // index table: [0-5] directly; [6] single indirect; [7] double indirect
    lba_index_t iblocks_[MAX_INODE_BLOCKS];
} __attribute__((packed)) inode_t;
extern inode_t __fs_inodes[MAX_INODES];

idx_t       inode_allocate(void);
void        inode_map_setup(idx_t inode_idx, bool is_set);
void        inode_map_update();
void        inode_set(idx_t inode_idx, uint32_t size, lba_index_t base_lba);
void        inodes_rw_disk(idx_t inode_idx, ata_cmd_t cmd);
void        iblock_set(idx_t inode_idx, idx_t iblock_idx, lba_index_t lba);
lba_index_t iblock_get(idx_t inode_idx, idx_t iblock_idx);
void        setup_inode(bool);

#endif
