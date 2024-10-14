/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DEVICE_CGA_CGA_H__
#define __KERN_DEVICE_CGA_CGA_H__

#include "boot/config_boot.h"
#include "kern/types.h"
#include "kern/x86.h"

#define CGA_REG_INDEX       0x3d4
#define CGA_REG_DATA        0x3d5
#define CGA_CURSOR_HIGH     0x0e
#define CGA_CURSOR_LOW      0x0f
#define VIDEO_MEM           (0xb8000+(KERN_HIGH_MAPPING))
#define CGA_WIDTH           80
#define CGA_HIGH            25
#define TO_POS(row, col)    ((row)*(CGA_WIDTH)+(col))
#define TO_ROW(pos)         ((pos)/(CGA_WIDTH))
#define TO_COL(pos)         ((pos)%(CGA_WIDTH))
#define SIZE_TAG            4

void cga_putc(char ch, uint8_t attr);
void cga_putstr(const char *, uint32_t);
void cga_putint(int);
void cga_puthex(uint32_t);

#endif
