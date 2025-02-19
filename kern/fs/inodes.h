#pragma once
#ifndef __KERN_FS_INODES_H__
#define __KERN_FS_INODES_H__

#include "super_block.h"
#include "kern/driver/ata/ata_cmd.h"

/**
 * @brief inode 类型
 */
typedef uint32_t inode_type_t;

#define INODE_TYPE_INVALID  0
#define INODE_TYPE_FILE     1
#define INODE_TYPE_DIR      2

/**
 * @brief inode
 */
typedef struct inode {
    uint32_t    size_;
    // index 索引表: [0-5] 直接索引; [6] 一级索引; [7] 两级索引
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
