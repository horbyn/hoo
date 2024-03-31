/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_SCHED_H__
#define __KERN_MODULE_SCHED_H__

#include "kern/mem/vm.h"

void kinit_tasks_system(void);
void kinit_idle_thread(void);

#endif
