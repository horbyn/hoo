/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UTILITIES_BITMAP_H__
#define __KERN_UTILITIES_BITMAP_H__

#include "kern/types.h"
#include "kern/driver/io.h"

/**
 * @brief bitmap structure definition
 */
typedef struct bitmap {
    // bitmap buffer length in bits
    uint32_t len_inbits_;
    uint8_t  *buff_;
    // previous index for free bit
    uint32_t prev_free_;
} __attribute__((packed)) bitmap_t;

void bitmap_init(bitmap_t *map, uint32_t len_inbits, void *buff);
bool bitmap_test(bitmap_t *map, int idx);
void bitmap_set(bitmap_t *map, int idx);
void bitmap_clear(bitmap_t *map, int idx);
int  bitmap_scan_empty(bitmap_t *map);

#endif
