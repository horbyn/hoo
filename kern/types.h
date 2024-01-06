/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_TYPES_H__
#define __KERN_TYPES_H__

#ifndef null
#define null  0
#endif

#ifndef bool
typedef enum { false = 0, true } bool;
#endif

#define NELEMS(x)       (sizeof(x) / sizeof((x)[0]))        // calculate the array size
#define BITS_PER_BYTE   8

typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned int   size_t;

typedef uint32_t idx_t;

#endif
