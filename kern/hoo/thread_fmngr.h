/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_HOO_THREAD_FMNGR_H__
#define __KERN_HOO_THREAD_FMNGR_H__

#include "kern/fs/fmngr.h"
#include "kern/sched/pcb.h"

void    init_thread_fmngr(pcb_t *pcb);
fmngr_t *thread_fmngr_get(tid_t tid);

#endif
