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

bool bitmap_test(void *, idx_t);
void bitmap_set(void *, idx_t);
void bitmap_clear(void *, idx_t);
uint32_t bitmap_scan(void *, uint32_t);

#endif
