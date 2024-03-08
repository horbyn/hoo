/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_DEBUG_H__
#define __KERN_DEBUG_H__

#include "kern/disp/disp.h"
#include "kern/sched/inte_stack.h"

extern void isr_part3();

void panic(const char *msg);
void trace(const char *extra);

#endif
