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

#define INODE_INDEX_ROOT            0                       // inode index where the root dir is

/**
 * @brief directory item
 */
typedef struct dir_item {
    inode_t *inode_;
    char     name_[16];                                     // 13.3 format
} dir_item_t;

#endif
