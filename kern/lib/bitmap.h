/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_LIB_BITMAP_H__
#define __KERN_LIB_BITMAP_H__

#include "kern/types.h"
#include "kern/debug.h"

#define BITMAP_GET_SIZE(bitmap_inbytes)  ((bitmap_inbytes) * BITS_PER_BYTE)

bool bitmap_test(void *, uint32_t, idx_t);
void bitmap_set(void *, uint32_t, idx_t);
void bitmap_clear(void *, uint32_t, idx_t);
uint32_t bitmap_scan(void *, uint32_t, idx_t, bool);

#endif
