/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __USER_BUILTIN_ULIB_H__
#define __USER_BUILTIN_ULIB_H__

#include "user.h"

#ifndef null
#define null  0
#endif

#ifndef bool
typedef enum { false = 0, true } bool;
#endif

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

/* string & memory operations */

uint32_t strlen(const char *str);
bool     strcmp(const char *a, const char *b);
void     memset(void *buff, uint8_t fill , uint32_t bufflen);
void     bzero(void *buff, uint32_t bufflen);
int      memmove(void *dst, const void *src, uint32_t size);

/* allocation & free */

void *alloc(uint32_t size);
void free(void *ptr);

/* system metadata */

int workingdir(char *wd, uint32_t len);

#endif
