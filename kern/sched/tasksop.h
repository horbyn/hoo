/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_TASKSOP_H__
#define __KERN_SCHED_TASKSOP_H__

#include "tasks.h"
#include "conf/Page.h"
#include "conf/preconf.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"
#include "kern/disp/disp.h"
#include "user/user.h"

extern void isr_part3();
extern void mode_ring3(uint32_t *, void *);
extern queue_t __queue_ready;
extern queue_t __queue_running;
extern spinlock_t __spinlock_disp;
extern uint32_t user;

void init_tasks_queue();
void init_disp_locks();
void kernel_idle_thread();
void user_init_thread();
void init_thread();

#endif
