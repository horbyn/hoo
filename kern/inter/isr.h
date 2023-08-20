/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_INTER_ISR_H__
#define __KERN_INTER_ISR_H__

#include "StuffInte.h"
#include "routine.h"

extern void syscall(void);

void init_isr_idt(void);
void set_idt_entry(privilege_t, int, uint32_t);
void set_isr_entry(int, uint32_t);

#endif
