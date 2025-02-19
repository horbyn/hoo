#pragma once
#ifndef __KERN_FS_FMNGR_H__
#define __KERN_FS_FMNGR_H__

#include "fs_stuff.h"
#include "kern/utilities/bitmap.h"

/**
 * @brief 文件管理器
 */
typedef struct file_manager {
    bitmap_t fd_set_;
    fd_t     *files_;
} fmngr_t;

void fmngr_init(fmngr_t *fmngr);
fd_t fmngr_alloc(fmngr_t *fmngr);
void fmngr_free(fmngr_t *fmngr, fd_t fd);
void fmngr_files_set(fmngr_t *fmngr, fd_t fd, fd_t val);
fd_t fmngr_files_get(fmngr_t *fmngr, fd_t fd);

#endif
