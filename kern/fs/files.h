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

extern files_t *__fs_files;

#ifdef DEBUG
    #include "kern/driver/io.h"
    void debug_print_files(void);
#endif
void files_init(void);
void files_create(const char *name);
void files_remove(const char *name);
fd_t files_open(const char *name);
void files_close(fd_t fd);

#endif
