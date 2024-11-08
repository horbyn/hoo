/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_FORMAT_H__
#define __KERN_UTILITIES_FORMAT_H__

typedef char *va_list;
#define POINTER_SIZE                (sizeof(const char *))
#define TYPE_TO_POINTER_SIZE(type)  ((sizeof(type) - 1) / POINTER_SIZE + 1)
#define VA_START(a,fst)             ((a) = (((va_list)&(fst))))
#define VA_ARG(a,type)              \
    ((a) = (a) + (sizeof(va_list) * TYPE_TO_POINTER_SIZE(type)))
#define VA_END(a)                   ((a) = (va_list)0)

void format(const char *fmt, va_list args, void *redirect);

#endif
