/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_SCHED_H__
#define __KERN_MODULE_SCHED_H__

#include "kern/sched/tasks.h"

extern void mode_ring3(uint32_t *user_stack, void *user_entry);

void kinit_tasks_system(void);
void thread_create(uint8_t *r0_top, uint8_t *r3_top, void *entry);

#endif
