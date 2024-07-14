/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_FILES_H__
#define __KERN_FS_FILES_H__

#include "dir.h"
#include "kern/types.h"

#define MAX_OPEN_FILES 1024

/**
 * @brief file descriptor
 */
typedef int fd_t;

/**
 * @brief file struct corresponding to the opening file.
 * that is there will be multiple structures if we
 * repeatly open a file
 */
typedef struct files {
    bool  used_;
    idx_t inode_idx_;
} __attribute__((packed)) files_t;
extern files_t *__fs_files;

void files_init(void);
void files_create(const char *name);
void files_remove(const char *name);
fd_t files_open(const char *name);
void files_close(fd_t fd);

#endif
