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
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"

void init_tasks_queue();
void kernel_idle_thread();
void kernel_init_thread();

#endif
