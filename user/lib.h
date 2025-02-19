#pragma once
#ifndef __USER_LIB_LIB_H__
#define __USER_LIB_LIB_H__

#include "user/types.h"

/* 字符串和内存操作 */

uint32_t strlen(const char *str);
bool     strcmp(const char *a, const char *b);
void     memset(void *buff, uint8_t fill , uint32_t bufflen);
void     bzero(void *buff, uint32_t bufflen);
int      memmove(void *dst, const void *src, uint32_t size);

/* 内存分配与释放 */

void *alloc(uint32_t size);
void free(void *ptr);

/* 系统数据 */

int workingdir(char *wd, uint32_t len);

#endif
