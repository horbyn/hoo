/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MEM_VM_H__
#define __KERN_MEM_VM_H__

#include "vmngr.h"

void *vir_alloc_pages(vsmngr_t *vmngr, uint32_t amount, uint32_t begin,
    uint32_t end);
void vir_release_pages(vsmngr_t *vmngr, void *va, bool rel);

#endif
