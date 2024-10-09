/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_TYPES_H__
#define __KERN_TYPES_H__

#include "user/ulib.h"

// calculate the array size
#define NELEMS(x)       (sizeof(x) / sizeof((x)[0]))
#define BITS_PER_BYTE   8

typedef int idx_t;
#define INVALID_INDEX (-1)

#endif
