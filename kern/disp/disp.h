/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_DISP_DISP_H__
#define __KERN_DISP_DISP_H__

#include "types.h"
#include "preconf.h"
#include "x86.h"
#include "lib/lib.h"

#define VIDEO_MEM               (0xb8000+KERN_HIGH_MAPPING) // video mm. base
#define VGA_WIDTH               80
#define VGA_HIGH                25
#define ROWCOL_TO_POS(row, col) ((row)*(VGA_WIDTH)+(col))
#define POS_TO_ROW(pos)         ((pos)/(VGA_WIDTH))
#define POS_TO_COL(pos)         ((pos)%(VGA_WIDTH))
#define SIZE_TAG                4
#define POINTER_SIZE            (sizeof(const char *))
#define TYPE_TO_POINTER_SIZE(type) \
    ((sizeof(type) - 1) / POINTER_SIZE + 1)
typedef char *va_list;
// set `a` to the address of `fst`
#define va_start(a,fst) \
    ((a) = (((va_list)&(fst))))
// move `a` to next pointer
#define va_arg(a,type)  \
    ((a) = (a) + (sizeof(va_list) * TYPE_TO_POINTER_SIZE(type)))
// set `a` to NULL (seems not necessary?)
#define va_end(a)       \
    ((a) = (va_list)0)

void        clear_screen(void);
void        set_cursor(int, int);
uint16_t    get_cursor(void);
void        scroll_back(void);
void        kprint_char(char);
void        kprint_str(const char *);
void        kprint_int(int);
void        kprint_hex(uint32_t);
void        kprintf(const char *, ...);

#endif
