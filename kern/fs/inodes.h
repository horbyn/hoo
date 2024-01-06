/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_INODES_H__
#define __KERN_FS_INODES_H__

#include "fs_stuff.h"
#include "kern/driver/ata_driver.h"

/**
 * @brief inode type
 */
typedef enum inode_type {
    INODE_TYPE_FILE = 0,
    INODE_TYPE_DIR = 1
} enum_inode_type;

/**
 * @brief definition of inode
 */
typedef struct inode {
    enum_inode_type type_;

    // index table: [0-5] directly; [6] single indirect; [7] double indirect
    lba_index_t     iblock_[MAX_INODE_BLOCKS];
} inode_t;

void setup_inode(bool);

#endif
