#pragma once
#ifndef __KERN_UTILITIES_BITMAP_H__
#define __KERN_UTILITIES_BITMAP_H__

#include "kern/x86.h"

/**
 * @brief 位图
 */
typedef struct bitmap {
    // 位图 buffer 长度，以比特为单位
    uint32_t len_inbits_;
    uint8_t  *buff_;
    // 前面查找过程中记录的下一个空闲的比特位
    uint32_t prev_free_;
} __attribute__((packed)) bitmap_t;

void bitmap_init(bitmap_t *map, uint32_t len_inbits, void *buff);
bool bitmap_test(bitmap_t *map, int idx);
void bitmap_set(bitmap_t *map, int idx);
void bitmap_clear(bitmap_t *map, int idx);
int  bitmap_scan_empty(bitmap_t *map);

#endif
