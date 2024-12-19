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
#include "kern/utilities/curdir.h"

// inode index where the root dir is
#define INODE_INDEX_ROOT    0
#define DIR_CUR             "."
#define DIR_PRE             ".."

/**
 * @brief directory item
 */
typedef struct dir_item {
    inode_type_t type_;
    int          inode_idx_;
    // 13.3 format
    char         name_[DIRITEM_NAME_LEN];
} diritem_t;

#define MAX_DIRITEM_PER_BLOCK   ((BYTES_SECTOR) / sizeof(diritem_t))

/**
 * @brief directory items to be filled into the block
 */
typedef struct dir_block {
    diritem_t dir_[MAX_DIRITEM_PER_BLOCK];
} dirblock_t;

void      diritem_set(diritem_t *dir, inode_type_t type, int inode_idx,
    const char *name);
bool      is_root_dir(const char *dir);
bool      diritem_find(const char *dir, diritem_t *found);
char      *diritem_traversal(diritem_t *dir);
diritem_t *diritem_create(inode_type_t type, const char *item_name, int parent_inode);
void      diritem_push(diritem_t *parent, diritem_t *cur);
int       diritem_remove(diritem_t *parent, diritem_t *cur);
diritem_t **get_root_dir(void);
int       dir_change(const char *dir);
int       dir_get_current(char *buff, uint32_t bufflen);

#endif
