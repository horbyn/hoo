/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_INODES_H__
#define __KERN_FS_INODES_H__

#include "free.h"

/**
 * @brief inode type
 */
typedef uint32_t inode_type_t;

#define INODE_TYPE_INVALID  0
#define INODE_TYPE_FILE     1
#define INODE_TYPE_DIR      2

/**
 * @brief definition of inode
 */
typedef struct inode {
    uint32_t    size_;
    // index table: [0-5] directly; [6] single indirect; [7] double indirect
    uint32_t iblocks_[MAX_INODE_BLOCKS];
} inode_t;
extern inode_t __fs_inodes[MAX_INODES];

void     setup_inode(bool is_new);
int      inode_allocate(void);
void     inode_map_setup(int inode_idx, bool is_set);
void     inode_map_update(void);
void     inode_set(int inode_idx, uint32_t size, uint32_t base_lba);
void     inodes_rw_disk(int inode_idx, atacmd_t cmd);
void     iblock_set(int inode_idx, int iblock_idx, uint32_t lba);
uint32_t iblock_get(int inode_idx, int iblock_idx);

#endif
