/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_DRIVER_H__
#define __KERN_MODULE_DRIVER_H__

#include "kern/driver/ata_driver.h"
#include "kern/driver/8042/8042.h"
#include "kern/driver/8259a/8259a.h"
#include "kern/driver/8253/8253.h"

void kinit_driver(void);

#endif
