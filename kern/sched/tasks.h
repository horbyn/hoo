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
extern void mode_ring3(uint32_t *user_stack, void *user_entry);

void  init_tasks_system(void);
pcb_t *get_current_pcb(void);
void  scheduler(void);
void  sleep(void *resource, spinlock_t *resource_lock);
void  wakeup(void *resource);
tid_t fork(void *entry);
void  wait_child(sleeplock_t *sl);
void  exit(void);
void  kill(void);

#endif
