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

#include "kern/types.h"
#include "kern/disp/disp.h"
#include "kern/sched/tasksop.h"

extern void scheduler(node_t *, node_t *);

void info(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
uint32_t, uint32_t, uint32_t);
void isr_default(void);
void divide_error(void);		                            // #0
void timer(void);				                            // #32

#endif
