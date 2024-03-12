/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DEVICE_CGA_CGA_H__
#define __KERN_DEVICE_CGA_CGA_H__

#include "kern/x86.h"
#include "kern/units/lib.h"

// video mm. base
#define VIDEO_MEM               (0xb8000+KERN_HIGH_MAPPING)
#define CGA_WIDTH               80
#define CGA_HIGH                25
#define ROWCOL_TO_POS(row, col) ((row)*(CGA_WIDTH)+(col))
#define POS_TO_ROW(pos)         ((pos)/(CGA_WIDTH))
#define POS_TO_COL(pos)         ((pos)%(CGA_WIDTH))
#define SIZE_TAG                4
#define CRT_CTL                 0x3d4

void        cga_clear_screen(void);
void        cga_putc(char);
void        cga_putstr(const char *, uint32_t);
void        cga_putint(int);
void        cga_puthex(uint32_t);

#endif
