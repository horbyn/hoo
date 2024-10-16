/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_MAIN_H__
#define __KERN_MODULE_MAIN_H__

#include "kern/fs/exec.h"
#include "kern/sched/tasks.h"
#include "user/user.h"

void ring3_first(void);

#endif
