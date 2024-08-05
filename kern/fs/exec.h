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

void exec(void (*prog_addr)(void));

#endif