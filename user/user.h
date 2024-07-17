/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __USER_USER_H__
#define __USER_USER_H__

#include "syscall_num.h"

extern void syscall_entry(int syscall_number, void *retval);

void printf(const char *format, ...);
void create(const char *filename);
void remove(const char *filename);
int  open(const char *filename);
void close(int fd);

#endif
