/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VM_H__
#define __KERN_MEM_VM_H__

#include "vaddr.h"
#include "kern/conf/page.h"
#include "kern/driver/io.h"
#include "kern/mem/pm.h"
#include "kern/module/idle.h"
#include "kern/units/format_list.h"
#include "kern/units/lib.h"
#include "kern/units/spinlock.h"
#include "kern/sched/tasks.h"

// the maximum value of idle thread virtual space -- the cause subtracted from
// 4-MB is that the last entry of its page directory table is not allowed to use
#define MAX_VSPACE_IDLE     (0xfffff000 - MB4 + 0x1000)

void init_virmm_system(void);
void *vir_alloc_pages(tid_t tid, vspace_t *vs, uint32_t amount);
void vir_release_pages(tid_t tid, vspace_t *vs, void *va);

#endif
