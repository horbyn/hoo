#pragma once
#ifndef __KERN_FS_FILES_H__
#define __KERN_FS_FILES_H__

#include "fs_stuff.h"
#include "user/types.h"

extern files_t *__fs_files;

void     filesystem_init(void);
int      files_create(const char *name);
int      files_remove(const char *name);
fd_t     files_open(const char *name);
void     files_close(fd_t fd);
void     files_read(fd_t fd, void *buf, uint32_t size);
void     files_write(fd_t fd, const char *buf, uint32_t size);
uint32_t files_get_size(fd_t fd);
int      files_list(const char *dir_or_file);

#endif
