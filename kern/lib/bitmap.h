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

bool bitmap_test(void *, idx_t);
void bitmap_set(void *, idx_t);
void bitmap_clear(void *, idx_t);

#endif
