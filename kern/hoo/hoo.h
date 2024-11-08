/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_HOO_HOO_H__
#define __KERN_HOO_HOO_H__

#include "thread_bucket.h"
#include "thread_fmngr.h"
#include "kern/desc/desc.h"
#include "kern/fs/fs_stuff.h"
#include "kern/page/page_stuff.h"
#include "kern/sched/pcb.h"
#include "kern/utilities/cache_buff.h"

#define PGDIR_HOO   ((SEG_PDTABLE) * 16 + (KERN_HIGH_MAPPING))

pgelem_t    *get_hoo_pgdir(void);
tss_t       *get_hoo_tss(void);
pcb_t       *get_hoo_pcb(void);
cachebuff_t *get_hoo_cache_buff(void);
void        init_hoo(void);

#endif
