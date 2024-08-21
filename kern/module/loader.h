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
#include "kern/fs/exec.h"
#include "kern/fs/files.h"

void load_builtins(void);

#endif
