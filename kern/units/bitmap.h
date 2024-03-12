/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_UNITS_BITMAP_H__
#define __KERN_UNITS_BITMAP_H__

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
    idx_t    prev_free_;
} __attribute__((packed)) bitmap_t;

void  bitmap_init(bitmap_t *map, uint32_t len_inbits, void *buff);
bool  bitmap_test(bitmap_t *map, idx_t idx);
void  bitmap_set(bitmap_t *map, idx_t idx);
void  bitmap_clear(bitmap_t *map, idx_t idx);
idx_t bitmap_scan_empty(bitmap_t *map);

#endif
