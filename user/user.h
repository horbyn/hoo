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

#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

void sys_printf(const char *format, ...);
void sys_create(const char *filename);
void sys_remove(const char *filename);
int  sys_open(const char *filename);
void sys_close(int fd);
void sys_read(int fd, void *buf, unsigned int count);
void sys_write(int fd, const void *buf, unsigned int count);
int  sys_fork(void);
void sys_exec(const char *program);
void sys_wait();
void sys_exit();
int  sys_workingdir(char *wd, unsigned int len);
int  sys_cd(const char *dir);
int  sys_ls(const char *dir_or_file);
void *sys_alloc(unsigned int size);
void sys_free(void *ptr);

#endif
