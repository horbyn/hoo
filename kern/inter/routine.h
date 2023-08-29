/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
/**
 * @file routine.h
 * @brief Install a isr routine:
 * 
 * 1. `inter/isr.c` to set `__isr[X]` pointing to any function;
 * 2. `inter/isrentry.s` to define macro and add it to `isr_part1[X]`;
 * 3. `inter/isr.c` to set `__idt[X]` pointing to `isr_part1[X]`
 */
#pragma once
#ifndef __KERN_INTER_ROUTINE_H__
#define __KERN_INTER_ROUTINE_H__

#include "conf/Config.h"
#include "kern/types.h"
#include "kern/assert.h"
#include "kern/disp/disp.h"
#include "kern/sched/tasksop.h"
#include "kern/syscall/Stuffsysc.h"

#define ISR32_TIMER             32
#define ISR128_SYSCALL          128

extern void scheduler(node_t *, node_t *);
extern Tss_t __tss;

void info(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t);
void isr_default(void);
void timer(void);				                            // #32
extern void syscall(void);                                  // #80

#endif
