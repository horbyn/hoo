/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_INTR_ROUTINE_H__
#define __KERN_INTR_ROUTINE_H__

#include "kern/x86.h"
#include "kern/driver/io.h"
#include "kern/sched/tasks.h"
#include "kern/driver/ata/ata_irq.h"

#define ISR14_PAGEFAULT         14
#define ISR32_TIMER             32
#define ISR46_HARD1             46
#define ISR128_SYSCALL          128

void isr_default(void);
void page_fault(void);
void timer(void);

#endif
