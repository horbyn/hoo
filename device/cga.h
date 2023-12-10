/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_CGA_H__
#define __DEVICE_CGA_H__

#include "conf/preconf.h"
#include "kern/debug.h"
#include "kern/types.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"

#define VIDEO_MEM               (0xb8000+KERN_HIGH_MAPPING) // video mm. base
#define CGA_WIDTH               80
#define CGA_HIGH                25
#define ROWCOL_TO_POS(row, col) ((row)*(CGA_WIDTH)+(col))
#define POS_TO_ROW(pos)         ((pos)/(CGA_WIDTH))
#define POS_TO_COL(pos)         ((pos)%(CGA_WIDTH))
#define SIZE_TAG                4
#define CRT_CTL                 0x3d4

void        clear_screen(void);
void        set_cursor(int, int);
uint16_t    get_cursor(void);
void        scroll_back(void);
void        cga_putc(char);
void        cga_putstr(const char *, size_t);
void        cga_putint(int);
void        cga_puthex(uint32_t);

#endif
