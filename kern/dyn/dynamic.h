#pragma once
#ifndef __KERN_MEM_DYNAMIC_H__
#define __KERN_MEM_DYNAMIC_H__

#include "kern/sched/tasks.h"

void *dyn_alloc(uint32_t size);
void dyn_free(void *ptr);

#endif
