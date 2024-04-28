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
#include "kern/mem/vm.h"
#include "kern/units/queue.h"

#define KERN_AVAIL_VMBASE   (KERN_HIGH_MAPPING + MM_BASE)

extern void switch_to(node_t *, node_t *);
extern void mode_ring3(uint32_t *user_stack, void *user_entry);
extern void idle(void);

pcb_t *get_current_pcb(void);
void  init_tasks_system(void);
tid_t allocate_tid(void);
void  scheduler();
void  task_ready(node_t *task);
void  init_idle(void);

#endif
