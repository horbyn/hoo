/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FILES_H__
#define __KERN_FS_FILES_H__

#include "dir.h"
#include "kern/types.h"

#define MAX_OPEN_FILES 1024

/**
 * @brief file struct corresponding to the opening file.
 * that is there will be multiple structures if we
 * repeatly open a file
 */
typedef struct files {
    idx_t inode_idx_;
} __attribute__((packed)) files_t;
extern files_t __fs_files[MAX_OPEN_FILES];

bool files_create(dir_item_t cur_dir, const char *name,
    void *buff, size_t bufflen);

#endif
