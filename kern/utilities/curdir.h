#pragma once
#ifndef __KERN_UTILITIES_CURDIR_H__
#define __KERN_UTILITIES_CURDIR_H__

#include "kern/page/page_stuff.h"
#include "user/types.h"

#define DIRNAME_ROOT_STR    "/"
#define DIRNAME_ROOT_ASCII  47 // '/' 的 ASCII 码
#define DIR_SEPARATOR       DIRNAME_ROOT_ASCII
#define DIRITEM_NAME_LEN    16
#define MAX_OPEN_DIR        (PGSIZE / (DIRITEM_NAME_LEN))
#define MAX_CURDIR_BUFF     PGSIZE

/**
 * @brief 当前目录
 */
typedef struct current_directory {
    char     *dir_;
    uint32_t dirlen_;
} curdir_t;

void curdir_init(curdir_t *curdir, char *dir, uint32_t dirlen);
int  curdir_get(const curdir_t *curdir, char *path, uint32_t pathlen);
int  curdir_set(curdir_t *curdir, const char *path);
void curdir_copy(curdir_t *dst, const curdir_t *src);
void get_parent_child_filename(char *path_to_parent, char *cur);

#endif
