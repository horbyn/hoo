/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_PRECONF_H__
#define __CONF_PRECONF_H__

#include "boot/kern_will_use.h"
#include "kern/types.h"

#define KERN_HIGH_MAPPING   0xc0000000
#define STACK_BOOT          0x80000
#define STACK_BOOT_SIZE     0x7c00
#define ADDR_ARDS_NUM       \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS_CR))             // the ARDS amount addr
#define ADDR_ARDS_BASE      \
    (((uint32_t)(SEG_ARDS))*16 + (OFF_ARDS))                // the ARDS itself addr
#define DIED_INSTRUCTION    \
    (((uint32_t)(SEG_DIED))*16 + (OFF_DIED))

#endif
