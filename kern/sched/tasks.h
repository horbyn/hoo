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
#include "kern/units/list.h"
#include "kern/units/queue.h"
#include "user/user.h"

// #define KERN_AVAIL_VMBASE   (KERN_HIGH_MAPPING + MM_BASE)
#define VIR_BASE_IDLE       0x1000

extern void switch_to(node_t *, node_t *);
extern void mode_ring3(uint32_t *user_stack, void *user_entry);

#ifdef DEBUG
    void debug_print_tasks(void);
    void debug_print_vspace(void);
    void debug_print_pgdir(void);
#endif
pcb_t *get_current_pcb(void);
void  init_tasks_system(void);
void  scheduler(void);
void  task_ready(node_t *task);
void  idle_init(void *entry);
tid_t fork(void *entry, sleeplock_t *sl);
void  sleep(sleeplock_t *slock);
void  wakeup(sleeplock_t *slock);
void  wait_sleeplock(tid_t tid_child);
void  signal_sleeplock(sleeplock_t *slock);
void  exit(void);
void  kill(pcb_t *pcb);
void  task_init_fmngr(fmngr_t *fmngr);

#endif
