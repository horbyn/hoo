/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_CURDIR_H__
#define __KERN_UTILITIES_CURDIR_H__

#include "kern/page/page_stuff.h"
#include "user/types.h"

#define DIRNAME_ROOT_STR    "/"
#define DIRNAME_ROOT_ASCII  47 // ascii code for '/'
#define DIR_SEPARATOR       DIRNAME_ROOT_ASCII
#define DIRITEM_NAME_LEN    16
#define MAX_OPEN_DIR        (PGSIZE / (DIRITEM_NAME_LEN))
#define MAX_CURDIR_BUFF     PGSIZE

/**
 * @brief current directory
 */
typedef struct current_directory {
    char     *dir_;
    uint32_t dirlen_;
    // point to current directory
    uint32_t ptr_cur_;
} curdir_t;

void curdir_init(curdir_t *curdir, char *dir, uint32_t dirlen);
int  curdir_get(const curdir_t *curdir, char *path, uint32_t pathlen);
int  curdir_set(curdir_t *curdir, const char *path);
void get_parent_child_filename(char *path_to_parent, char *cur);

#endif
