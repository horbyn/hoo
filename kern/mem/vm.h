/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VM_H__
#define __KERN_MEM_VM_H__

#include "vspace.h"
#include "kern/sched/pcb.h"

void init_virmm_system(void);
void *vir_alloc_pages(vspace_t *vspace, uint32_t amount, uint32_t begin);
void vir_release_pages(vspace_t *vspace, void *va, bool rel);
void release_vspace(pcb_t *pcb);

#endif
