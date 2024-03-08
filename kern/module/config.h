/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MODULE_CONFIG_H__
#define __KERN_MODULE_CONFIG_H__

#include "kern/mem/gdt.h"
#include "kern/mem/tss.h"
#include "kern/mem/mmu.h"

#define SIZE_GDT    8                                       // the gdt descriptors amount

extern Tss_t __tss;

void kernel_config(void);

#endif
