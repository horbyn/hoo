#pragma once
#ifndef __KERN_HOO_THREAD_PCB_H__
#define __KERN_HOO_THREAD_PCB_H__

#include "kern/sched/pcb.h"

void  init_thread_pcb(pcb_t *pcb);
pcb_t *thread_pcb_get(tid_t tid);

#endif
