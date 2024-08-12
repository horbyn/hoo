/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_LOADER_H__
#define __KERN_MODULE_LOADER_H__

#include "kern/x86.h"
#include "kern/fs/files.h"
#include "user/builtin_ls.h"

#define DIR_LOADER  "/bin/"

extern uint8_t __ls_base[], __ls_end[];

void load_builtins(void);

#endif
