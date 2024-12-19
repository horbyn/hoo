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

void sys_create(const char *filename);
void sys_remove(const char *filename);
int  sys_open(const char *filename);
void sys_close(int fd);
void sys_read(int fd, void *buf, unsigned int count);
void sys_write(int fd, const void *buf, unsigned int count);
void sys_printf(const char *format, ...);
int  sys_fork(void);
void sys_wait();
void sys_exit();
int  sys_cd(const char *dir);
void sys_exec(const char *program);
int  sys_ls(const char *dir_or_file);
void *sys_alloc(unsigned int size);
void sys_free(void *ptr);
int  sys_workingdir(char *wd, unsigned int len);

#endif
