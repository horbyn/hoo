/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_CONF_CONFIG_H__
#define __KERN_CONF_CONFIG_H__

#include "descriptor.h"
#include "page.h"
#include "kern/driver/io.h"
#include "kern/mem/mm.h"

#define SIZE_GDT    8

void kernel_config(void);

#endif
