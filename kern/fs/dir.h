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
#include "kern/lib/bitmap.h"

/**
 * @brief directory item
 */
typedef struct dir_item {
    enum_inode_type type_;
    inode_t        *inode_;
    char            name_[16];                              // 13.3 format
} dir_item_t;

void set_dir_item(dir_item_t *dir, enum_inode_type type, inode_t *inode, const char *name);
void setup_root_dir(void);

#endif
