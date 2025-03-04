#pragma once
#ifndef __KERN_FS_FS_STUFF_H__
#define __KERN_FS_FS_STUFF_H__

#include "kern/x86.h"
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
#define LBA_ITEMS_PER_SECTOR        ((BYTES_SECTOR) / sizeof(uint32_t))
#define MAX_FILES_PER_TASK          64
#define MAX_OPEN_FILES              ((MAX_FILES_PER_TASK) * (MAX_TASKS_AMOUNT))

/**
 * @brief 文件描述符
 */
typedef int fd_t;

/**
 * @brief 用来表示打开的文件，如果重复打开一个文件，会有多个结构体
 */
typedef struct files {
    int      inode_idx_;
    uint32_t ref_;
} files_t;

#endif
