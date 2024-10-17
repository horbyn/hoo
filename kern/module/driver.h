/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_DRIVER_H__
#define __KERN_MODULE_DRIVER_H__

#include "kern/driver/8259a/8259a.h"
#include "kern/driver/8253/8253.h"

#define TICKS_PER_SEC   1000

void kinit_driver(void);

#endif
