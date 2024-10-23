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

// inode index where the root dir is
#define INODE_INDEX_ROOT    0
#define DIRITEM_NAME_LEN    16
#define DIRNAME_ROOT_ASCII  47 // ascii code for '/'
#define DIRNAME_ROOT_STR    "/"

/**
 * @brief directory item
 */
typedef struct dir_item {
    inode_type_t type_;
    int          inode_idx_;
    // 13.3 format
    char         name_[DIRITEM_NAME_LEN];
} diritem_t;

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
} dirblock_t;

void      diritem_set(diritem_t *dir, inode_type_t type, int inode_idx,
    const char *name);
bool      is_root_dir(const char *dir);
int       diritem_write(dirblock_t *block, const diritem_t *item);
diritem_t *diritem_read(dirblock_t *block, const diritem_t *item);
bool      diritem_find(const char *dir, diritem_t *found);
void      dirblock_get_new(dirblock_t *result, int self, int parent);
void      setup_root_dir(bool is_new);

#endif
