/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_IDLE_H__
#define __KERN_MODULE_IDLE_H__

#include "kern/conf/page.h"
#include "kern/sched/tasks.h"

extern pcb_t *__pcb_idle;
extern pgelem_t __pgdir_idle[];

#endif
