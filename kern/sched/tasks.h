/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_SCHED_TASKS_H__
#define __KERN_SCHED_TASKS_H__

#include "kern/x86.h"
#include "kern/conf/descriptor.h"
#include "kern/module/idle.h"
#include "kern/units/queue.h"

extern void switch_to(node_t *, node_t *);

queue_t *get_idle_ready_queue(void);
queue_t *get_idle_running_queue(void);
pcb_t   *get_current_pcb(void);
void    init_tasks_system(void);
tid_t   allocate_tid(void);
void    scheduler();

#endif
