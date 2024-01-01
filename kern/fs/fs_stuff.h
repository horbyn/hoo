/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FS_STUFF_H__
#define __KERN_FS_FS_STUFF_H__

#include "kern/types.h"

#define MAX_INODE                       64
#define MAX_INODE_BLOCKS                8
#define FS_LAYOUT_SECTORS_PARTITION     1
#define FS_LAYOUT_SECTORS_SUPERBLOCK    2
#define FS_LAYOUT_SECTORS_MAP_INODES    1
#define FS_LAYOUT_SECTORS_MAP_FREE      1
#define FS_LAYOUT_SECTORS_ROOT          1

/**
 * @brief the size of LBA index
 */
typedef uint32_t lba_index_t;

/**
 * @brief inode type
 */
typedef enum inode_type {
    INODE_TYPE_FILE = 0,
    INODE_TYPE_DIR = 1
} enum_inode_type;

/**
 * @brief inode level
 */
typedef enum inode_level {
    INODE_LEVEL0 = 0,                                       // direct
    INODE_LEVEL1,                                           // single indirect
    INODE_LEVEL2                                            // double indirect
} enum_indoe_level_t;

/**
 * @brief definition of inode
 */
typedef struct inode {
    enum_inode_type type_;

    // index table: [0-5] directly; [6] single indirect; [7] double indirect
    lba_index_t     iblock_[MAX_INODE_BLOCKS];
} inode_t;

/**
 * @brief directory item
 */
typedef struct dir_item {
    inode_t *inode_;
    char     name_[16];                                     // 13.3 format
} dir_item_t;

/**
 * @brief super block
 */
typedef struct super_block {
    lba_index_t lba_partition_,
        lba_super_block_,
        lba_map_inode_,
        lba_map_free_,
        lba_inodes_,
        lba_root_,
        lba_free_;
} super_block_t;

#endif
