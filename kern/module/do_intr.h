/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_DO_INTR_H__
#define __KERN_MODULE_DO_INTR_H__

#include "kern/x86.h"
#include "kern/intr/intr.h"
#include "kern/intr/routine.h"

// the entrance of isr
extern uint32_t isr_part1[IDT_ENTRIES_NUM];
// the isr routines
extern isr_t __isr[];

void kinit_isr_idt(void);

#endif
