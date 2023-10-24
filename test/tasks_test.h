/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __TEST_TASKS_TEST_H__
#define __TEST_TASKS_TEST_H__

#include "conf/Page.h"
#include "conf/preconf.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"
#include "kern/disp/disp.h"
#include "kern/sched/tasks.h"
#include "user/user.h"

void init_thread();
void test_kernel_threads();

#endif
