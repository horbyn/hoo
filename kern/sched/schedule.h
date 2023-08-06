/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_SCHED_INIT_H__
#define __KERN_SCHED_INIT_H__

#include "tasks.h"
#include "conf/Page.h"
#include "conf/preconf.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"
#include "kern/disp/disp.h"

extern void isr_part3();
extern queue_t __queue_ready;
extern queue_t __queue_running;

void init_tasks_queue();
void kernel_idle_thread();
void kernel_init_thread();
void init_thread();

#endif
