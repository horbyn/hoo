/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_INODES_H__
#define __KERN_FS_INODES_H__

#include "super_block.h"

#define INODE_INDEX_ROOT            0                       // inode index where the root dir is

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
    size_t      size_;
    // index table: [0-5] directly; [6] single indirect; [7] double indirect
    lba_index_t iblock_[MAX_INODE_BLOCKS];
} inode_t;

extern inode_t __fs_inodes[MAX_INODES];

lba_index_t inode_allocate(void);
void set_inode(inode_t *inode, size_t size, lba_index_t base_lba);
void inode_to_disk(lba_index_t lba);
void setup_inode(bool);

#endif
