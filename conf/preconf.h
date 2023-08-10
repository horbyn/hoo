/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_PRECONF_H__
#define __CONF_PRECONF_H__

/************************************
 * Following definition MUST be the *
 * same as `boot/kern_will_use.inc` *
 ************************************/

#define KERN_HIGH_MAPPING   0xc0000000
#define STACK_BOOT          0x80000
#define STACK_BOOT_SIZE     0x7c00
#define ADDR_ARDS_NUM       0x78200                         // the ARDS amount addr
#define ADDR_ARDS_BASE      0x78204                         // the ARDS itself addr
#define DIED_INSTRUCTION    0x77ffc

#endif
