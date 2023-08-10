/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MODULE_SCHED_H__
#define __KERN_MODULE_SCHED_H__

#include "kern/sched/tasksop.h"
#include "kern/lib/spinlock.h"
#include "kern/disp/disp.h"

void init_scheduler();
void init_locks();

#endif
