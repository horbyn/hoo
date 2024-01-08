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
    inode_t        *inode_;
    char            name_[16];                              // 13.3 format
} dir_item_t;

void set_dir_item(dir_item_t *dir, enum_inode_type type,
    inode_t *inode, const char *name);
void create_root_dir(void);

#endif
