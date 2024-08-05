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

void sys_printf(const char *format, ...);
void sys_create(const char *filename);
void sys_remove(const char *filename);
int  sys_open(const char *filename);
void sys_close(int fd);
void sys_read(int fd, void *buf, unsigned int count);
void sys_write(int fd, const void *buf, unsigned int count);
int  sys_fork(void);

#endif
