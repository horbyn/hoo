/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MODULE_DEVICE_H__
#define __KERN_MODULE_DEVICE_H__

#include "device/ata/ata.h"
#include "device/8259a.h"
#include "device/8253.h"
#include "kern/types.h"

void init_pic(void);
void init_pit(void);
void init_ata(void);

#endif
