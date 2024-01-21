/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_DIR_H__
#define __KERN_FS_DIR_H__

#include "inodes.h"
#include "free.h"
#include "kern/lib/bitmap.h"

#define FS_DIR_BASIC_CUR    0
#define FS_DIR_BASIC_PRE    1

/**
 * @brief directory item
 */
typedef struct dir_item {
    enum_inode_type type_;
    idx_t           inode_idx_;
    char            name_[16];                              // 13.3 format

    // padding
    uint32_t        :32;
    uint32_t        :32;
} __attribute__((packed)) dir_item_t;

#define MAX_DIRITEM_PER_BLOCK \
    ((BYTES_SECTOR) / sizeof(dir_item_t))
/**
 * @brief directory items to be filled into the block
 */
typedef struct dir_block {
    uint32_t   amount_;                                     // the amount of the block
    uint32_t   :32;
    dir_item_t dir_[MAX_DIRITEM_PER_BLOCK];
} __attribute__((packed)) dir_block_t;

void set_dir_item(dir_item_t *dir, enum_inode_type type,
    idx_t inode_idx, const char *name);
void set_dir_block(dir_block_t *blocks, size_t block_size,
    const dir_item_t *items, uint32_t item_amount);
void dirs_write_disk(dir_item_t *dest, dir_item_t *to_write, uint32_t amount);
void setup_root_dir(bool is_new);

#endif
