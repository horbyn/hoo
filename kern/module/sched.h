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

void kinit_tasks_system(void);
void kthread_create(uint8_t *r0_top, uint8_t *r3_top, void *entry);

#endif
