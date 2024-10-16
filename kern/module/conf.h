/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_CONF_H__
#define __KERN_MODULE_CONF_H__

#include "kern/desc/gdt.h"
#include "kern/page/page.h"

#define SIZE_GDT    8

void kinit_config(void);

#endif
