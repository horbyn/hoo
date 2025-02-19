#pragma once
#ifndef __KERN_FS_EXEC_H__
#define __KERN_FS_EXEC_H__

#include "files.h"

#define DIR_LOADER      "/bin/"
#define BUILT_SHELL     "shell"
#define BUILT_CD        "cd"
#define BUILT_LS        "ls"
#define BUILT_PWD       "pwd"
#define BUILT_MKDIR     "mkdir"
#define BUILT_TOUCH     "touch"
#define BUILT_RM        "rm"
#define MAXSIZE_PATH    128
#define MAX_ARGV        2

// 内置命令的函数原型
typedef void (*builtin_t)(void);

void exec(const char *filename);

#endif
