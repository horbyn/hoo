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
#include "kern/module/hoo.h"

// the maximum value of hoo thread virtual space -- the cause subtracted from
// 4-MB is that the last entry of its page directory table is not allowed to use
#define MAX_VSPACE_HOO      (PG_MASK - MB4 + PGSIZE)

#ifdef DEBUG
    void debug_print_vspace_pcb(pcb_t *pcb);
#endif
void init_virmm_system(void);
void *vir_alloc_pages(pcb_t *pcb, uint32_t amount);
void vir_release_pages(pcb_t *pcb, void *va, bool rel);
void phy_release_vpage(pcb_t *pcb, void *page_vir_addr);
void release_vspace(pcb_t *pcb);

#endif
