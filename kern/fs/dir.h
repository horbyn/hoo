/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_DIR_H__
#define __KERN_FS_DIR_H__

#include "inodes.h"
#include "free.h"
#include "kern/units/lib.h"

#define DIRITEM_NAME_LEN    16
#define DIRNAME_ROOT        "/"

/**
 * @brief directory item
 */
typedef struct dir_item {
    enum_inode_type type_;
    idx_t           inode_idx_;
    // 13.3 format
    char            name_[DIRITEM_NAME_LEN];
} __attribute__((packed)) diritem_t;

#define MAX_DIRITEM_PER_BLOCK \
    ((BYTES_SECTOR - 2 * sizeof(uint32_t)) / sizeof(diritem_t))

/**
 * @brief directory items to be filled into the block
 */
typedef struct dir_block {
    // the amount of the directory items
    uint32_t   amount_;
    uint32_t   :32;
    diritem_t dir_[MAX_DIRITEM_PER_BLOCK];
} __attribute__((packed)) dirblock_t;

void      diritem_set(diritem_t *dir, enum_inode_type type, idx_t inode_idx,
    const char *name);
int       diritem_write(dirblock_t *block, const diritem_t *item);
diritem_t *diritem_read(dirblock_t *block, const diritem_t *item);
void      diritem_find(const char *dir, diritem_t *found);
void      dirblock_get_new(dirblock_t *result);
void      setup_root_dir(bool is_new);

#endif
