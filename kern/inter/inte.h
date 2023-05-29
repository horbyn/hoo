/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_INTER_INTE_H__
#define __KERN_INTER_INTE_H__

#include "StuffInte.h"
#include "disp/disp.h"

void init_interrupt(void);
void set_idt_entry(int, uint32_t);
void set_isr_entry(int, uint32_t);

// =========================================================
// ======================== isr ============================
// =========================================================

void info(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t);
void isr_default(void);
void divide_error(void);		                            // #0
void timer(void);				                            // #32

#endif
