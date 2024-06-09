/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_DYNAMIC_H__
#define __KERN_MEM_DYNAMIC_H__

#include "bucket_mngr.h"
#include "kern/driver/io.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"
#include "kern/sched/tasks.h"

void *dyn_alloc(uint32_t size);
void dyn_free(void *ptr);

#endif
