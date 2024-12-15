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
#define LASTLINE_BEG        (TO_POS(CGA_HIGH - 1, 0))
#define LASTLINE_END        (TO_POS(CGA_HIGH - 1, CGA_WIDTH - 1))
#define WHITH_CHAR          (0x0f00 | ' ')
#define DEF_IOATTR          0xf

/**
 * @brief cga color attribute
 */
typedef uint32_t color_t;

#define SCBUFF_COLOR_BLACK      0
#define SCBUFF_COLOR_BLUE       1
#define SCBUFF_COLOR_GREEN      2
#define SCBUFF_COLOR_CYAN       3
#define SCBUFF_COLOR_RED        4
#define SCBUFF_COLOR_MAGENTA    5
#define SCBUFF_COLOR_YELLOW     6
#define SCBUFF_COLOR_WHITE      7

/**
 * @brief cga brightness attribute
 */
typedef uint32_t bright_t;

#define SCBUFF_STYLE_NORMAL  0
#define SCBUFF_STYLE_LIGHT   1

void cga_init(void);
void cga_clear(void);
void cga_set_attribute(color_t color, bright_t bright);
void cga_putstr(const char *str, uint32_t len);

#endif
