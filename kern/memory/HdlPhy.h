/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEMORY_HDLPHY_H__
#define __KERN_MEMORY_HDLPHY_H__

#include "StuffPhy.h"
#include "lib/lib.h"

/************************************
 * Following definition MUST be the *
 * same as `boot/kern_will_use.inc` *
 ************************************/

#define ADDR_ARDS_NUM   0x78200                             // the ARDS amount addr
#define ADDR_ARDS_BASE  0x78204                             // the ARDS itself addr

#define MM_BASE         0x100000                            // memory will be traced
#define PGSIZE          4096
#define PGDOWN(x, align) \
    ((x) & ~(align - 1))
#define PGUP(x, align) \
    (PGDOWN((x + align - 1), align))

void init_phymm(void);

#endif
