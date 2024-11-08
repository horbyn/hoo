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
#include "kern/utilities/node.h"

extern void switch_to(node_t *, node_t *);

void  init_tasks_system(void);
pcb_t *get_current_pcb(void);
void  scheduler(void);

#endif
