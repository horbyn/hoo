/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_IDLE_H__
#define __KERN_MODULE_IDLE_H__

#include "kern/conf/descriptor.h"
#include "kern/conf/page.h"
#include "kern/sched/pcb.h"

pgelem_t *get_idle_pgdir(void);
tss_t    *get_idle_tss(void);
pcb_t    *get_idle_pcb(void);

#endif
