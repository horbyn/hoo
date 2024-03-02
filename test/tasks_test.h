/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __TEST_TASKS_TEST_H__
#define __TEST_TASKS_TEST_H__

#include "kern/mem/page.h"
#include "kern/module/config.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"
#include "kern/disp/disp.h"
#include "kern/sched/tasks.h"
#include "user/user.h"

void init_thread();
void test_kernel_threads();

#endif
