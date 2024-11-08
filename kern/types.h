/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_TYPES_H__
#define __KERN_TYPES_H__

#ifndef bool
typedef enum { false = 0, true } bool;
#endif

// the null pointer in kernel is not zero but a special address;
//   while in user space is zero
extern void *null;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef uint32_t       tid_t;

// calculate the array size
#define NELEMS(x)       (sizeof(x) / sizeof((x)[0]))

#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

#endif
