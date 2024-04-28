/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SYSCALL_SYSCALL_H__
#define __KERN_SYSCALL_SYSCALL_H__

#include "kern/driver/io.h"

#define MAX_SYSCALL 32
typedef void (*syscall_t)(void);
extern syscall_t __stub[MAX_SYSCALL];

#endif
