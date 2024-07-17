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

#define MAX_FILES_PER_TASK  64
#define MAX_OPEN_FILES      ((MAX_FILES_PER_TASK) * (MAX_TASKS_AMOUNT))

/**
 * @brief file descriptor
 */
typedef int fd_t;

/**
 * @brief file struct corresponding to the opening file.
 * that is there will be multiple structures if we
 * repeatly open a file
 */
typedef struct files {
    bool     used_;
    idx_t    inode_idx_;
    uint32_t ref_;
} __attribute__((packed)) files_t;

#endif
