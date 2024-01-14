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
#include "blocks.h"
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
} __attribute__((packed)) dir_item_t;

void set_dir_item(dir_item_t *dir, enum_inode_type type,
    idx_t inode_idx, const char *name);
void setup_root_dir(bool is_new);

#endif
