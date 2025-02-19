#pragma once
#ifndef __KERN_HOO_THREAD_CURDIR_H__
#define __KERN_HOO_THREAD_CURDIR_H__

#include "kern/sched/pcb.h"
#include "kern/utilities/curdir.h"

void     init_thread_curdir(pcb_t *pcb);
curdir_t *thread_curdir_get(tid_t tid);
void     thread_curdir_clear(tid_t tid);

#endif
