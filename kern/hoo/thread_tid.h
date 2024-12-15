/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_HOO_THREAD_TID_H__
#define __KERN_HOO_THREAD_TID_H__

#include "kern/sched/pcb.h"

void  init_thread_tid(pcb_t *pcb);
tid_t thread_tid_alloc(void);
void  thread_tid_release(tid_t tid);

#endif
