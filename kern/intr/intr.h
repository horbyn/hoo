/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_INTR_INTR_H__
#define __KERN_INTR_INTR_H__

#include "intr_stuff.h"

void set_idt_entry(idt_t *idt, privilege_t pvl, gatedesc_t gate, uint32_t addr);
void set_isr_entry(isr_t *isr, isr_t addr);

#endif
