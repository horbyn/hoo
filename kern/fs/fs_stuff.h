/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FS_STUFF_H__
#define __KERN_FS_FS_STUFF_H__

#include "kern/types.h"

#define MAX_INODE_BLOCKS            8
#define MAX_INODES                  64
#define FS_HOO_MAGIC                0x1905e14d
#define FS_LAYOUT_BASE_PARTITION    0
#define FS_LAYOUT_BASE_SUPERBLOCK   1
#define FS_LAYOUT_BASE_MAP_INODES   2
#define FS_LAYOUT_BASE_INODES       3
#define FS_LAYOUT_BASE_MAP_FREE     ((FS_LAYOUT_BASE_INODES) + (MAX_INODES))

/**
 * @brief the size of LBA index
 */
typedef uint32_t lba_index_t;

/**
 * @brief index level
 */
typedef enum index_level {
    INDEX_LEVEL0 = 0,                                       // direct
    INDEX_LEVEL1,                                           // single indirect
    INDEX_LEVEL2,                                           // double indirect

    MAX_FS_ACCESS_LEVEL
} enum_index_level_t;
#define MACRO_STRING_INDEX_LEVEL(e) \
    ((e == INDEX_LEVEL0) ? "direct" : \
     (e == INDEX_LEVEL1) ? "single indirect" : \
     (e == INDEX_LEVEL2) ? "double indirect" : "unknown")

#endif
