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
#define BUILT_PWD       "pwd"
#define BUILT_LS        "ls"
#define BUILT_CD        "cd"
#define MAXSIZE_PATH    128
#define MAX_ARGV        16

// prototype of all the builtin commands
typedef void (*builtin_t)(void);

void exec(const char *filename);

#endif