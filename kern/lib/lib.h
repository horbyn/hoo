/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_LIB_LIB_H__
#define __KERN_LIB_LIB_H__

#include "kern/x86.h"

uint32_t strlen(const char *str);
void     memset(void *buff, uint8_t fill , uint32_t bufflen);
void     bzero(void *buff, uint32_t bufflen);
int      memmove(void *dst, const void *src, uint32_t size);

#endif
