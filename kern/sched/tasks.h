/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

#include "kern/hoo/hoo.h"
#include "kern/mem/vm.h"
#include "kern/utilities/queue.h"

extern void switch_to(node_t *, node_t *);

pcb_t *get_current_pcb(void);
void  init_tasks_system(void);
void  scheduler(void);

#endif
