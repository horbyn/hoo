/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
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
#include "device/ata/ata.h"
#include "kern/types.h"
#include "kern/debug.h"
#include "kern/disp/disp.h"
#include "kern/sched/tasks.h"
#include "kern/syscall/Stuffsysc.h"

#define ISR32_TIMER             32
#define ISR38_FLOPPY            38
#define ISR46_HARD1             46
#define ISR128_SYSCALL          128

extern void switch_to(node_t *, node_t *);
extern Tss_t __tss;

void info(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t);
void isr_default(void);
void timer(void);				                            // #32
void floppy_driver(void);				                    // #38
extern void ata_irq_intr(void);                             // #46
extern void syscall(void);                                  // #80

#endif
