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
extern void idle(void);

void kinit_tasks_system(void);
void idle_enter_ring3(void);

#endif
