/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_KERNEL_H__
#define __KERN_KERNEL_H__

#include "x86.h"
#include "kern/disp/disp.h"
#include "kern/module/device.h"
#include "kern/module/inte.h"
#include "kern/module/sched.h"
#include "kern/sched/tasks.h"
#include "test/tasks_test.h"
#include "test/disk_rw_test.h"

extern uint8_t __kern_base[], __kern_end[];

void kernel_exec(void);
void kernel_init(void);

#endif
