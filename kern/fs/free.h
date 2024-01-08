/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FREE_H__
#define __KERN_FS_FREE_H__

#include "super_block.h"
#include "kern/lib/bitmap.h"

uint32_t allocate_free_block();
void setup_free_map(bool is_new);

#endif
