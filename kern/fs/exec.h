/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_FS_EXEC_H__
#define __KERN_FS_EXEC_H__

#include "files.h"

#define DIR_LOADER      "/bin/"
#define BUILT_SHELL     "shell"
#define BUILT_LS        "ls"
#define MAXSIZE_PATH    512

// prototype of all the builtin commands
typedef void (*builtin_t)(void);

int exec(const char *filename);

#endif