/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_8042_8042_H__
#define __KERN_DRIVER_8042_8042_H__

#include "cmd_8042.h"
#include "kern/driver/io.h"

void ps2_intr(void);

#endif
