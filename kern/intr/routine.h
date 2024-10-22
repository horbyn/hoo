/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_INTR_ROUTINE_H__
#define __KERN_INTR_ROUTINE_H__

#include "kern/types.h"
#include "kern/driver/io.h"
#include "kern/sched/tasks.h"

#define ISR32_TIMER             32

void isr_default(void);
void timer(void);

#endif
