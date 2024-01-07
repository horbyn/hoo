/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FREE_H__
#define __KERN_FS_FREE_H__

#include "fs_stuff.h"
#include "kern/lib/bitmap.h"
#include "kern/driver/ata_driver.h"

/**
 * @brief used to calculate free bitmap and free blocks
 */
typedef struct free_layout {
    enum_index_level_t level_;
    uint32_t bitmap_free_;                                  // sectors amount of free bitmap layout
    uint32_t free_;                                         // sectors amount of free block layout
} free_layout_t;

void init_free_layout(void);
const free_layout_t *get_free_layout(uint32_t dev_sec);
uint32_t free_block_allocate(const free_layout_t *free_layout);
void setup_free_map(bool is_new);

#endif
