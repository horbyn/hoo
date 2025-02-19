#pragma once
#ifndef __USER_TYPES_H__
#define __USER_TYPES_H__

#ifndef bool
typedef enum { false = 0, true } bool;
#endif

// null 指针在内核态不是 0 而是一个特殊的地址，而在用户态是 0
extern void *null;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef uint32_t       tid_t;

// 计算数组长度
#define NELEMS(x)       (sizeof(x) / sizeof((x)[0]))

#define FD_STDIN    0
#define FD_STDOUT   1
#define FD_STDERR   2

#endif
