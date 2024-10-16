/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_CONFIG_H__
#define __KERN_MODULE_CONFIG_H__

#include "hoo.h"
#include "kern/conf/descriptor.h"
#include "kern/driver/io.h"
#include "kern/mem/pm.h"

#define SIZE_GDT    8

void kinit_config(void);

#endif
