/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_IO_H__
#define __KERN_DRIVER_IO_H__

#include "cga/cga.h"
#include "kern/conf/page.h"
#include "kern/units/spinlock.h"
#include "kern/intr/intr_stack.h"

extern void isr_part3(void);

#define POINTER_SIZE                (sizeof(const char *))
#define TYPE_TO_POINTER_SIZE(type)  ((sizeof(type) - 1) / POINTER_SIZE + 1)
typedef char *va_list;
// set `a` to the address of `fst`
#define va_start(a,fst)             ((a) = (((va_list)&(fst))))
// move `a` to next pointer
#define va_arg(a,type)              ((a) = (a) + (sizeof(va_list) * TYPE_TO_POINTER_SIZE(type)))
// set `a` to NULL (seems not necessary?)
#define va_end(a)                   ((a) = (va_list)0)

void io_init(void);
void clear_screen(void);
void kprintf(const char *fmt, ...);
void panic(const char *extra);

#endif
