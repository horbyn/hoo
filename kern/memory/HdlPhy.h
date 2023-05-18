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
#include "x86.h"

/************************************
 * Following definition MUST be the *
 * same as `boot/kern_will_use.inc` *
 ************************************/

#define ADDR_ARDS_NUM   0x78200                             // the ARDS amount addr
#define ADDR_ARDS_BASE  0x78204                             // the ARDS itself addr

void init_phymm(void);

#endif
