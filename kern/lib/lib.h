/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_LIB_H__
#define __KERN_LIB_LIB_H__

#include "types.h"

uint32_t strlen(const char *);
void memset(void *, uint8_t, size_t);
void bzero(void *, size_t);
int memmove(void *, const void *, size_t);

#endif
