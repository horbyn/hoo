/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_FS_STUFF_H__
#define __KERN_FS_FS_STUFF_H__

#include "kern/types.h"
#include "kern/driver/ata/ata_stuff.h"

#define MAX_DIRECT                  6
#define MAX_INODE_BLOCKS            8
#define MAX_INODES                  64
#define FS_HOO_MAGIC                0x1905e14d
#define FS_LAYOUT_BASE_PARTITION    0
#define FS_LAYOUT_BASE_SUPERBLOCK   1
#define FS_LAYOUT_BASE_MAP_INODES   2
#define FS_LAYOUT_BASE_INODES       3
#define FS_LAYOUT_BASE_MAP_FREE     ((FS_LAYOUT_BASE_INODES) + (MAX_INODES))
#define LBA_ITEMS_PER_SECTOR        ((BYTES_SECTOR) / sizeof(lba_index_t))

/**
 * @brief the size of LBA index
 */
typedef uint32_t lba_index_t;

#endif