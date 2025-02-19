#pragma once
#ifndef __KERN_MODULE_DO_INTR_H__
#define __KERN_MODULE_DO_INTR_H__

#include "kern/intr/intr.h"

// ISR 入口
extern uint32_t isr_part1[IDT_ENTRIES_NUM];
// ISR 数组
extern isr_t __isr[];
extern void syscall(void);

void kinit_isr_idt(void);

#endif
