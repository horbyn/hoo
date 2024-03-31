/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_MEM_H__
#define __KERN_MODULE_MEM_H__

#include "kern/x86.h"
#include "kern/mem/pm.h"
#include "kern/mem/vm.h"

/**
 * @brief physical memory information
 */
typedef struct mem_info {
    uint32_t base_;
    uint32_t length_;
} mminfo_t;

void kinit_memory();
const mminfo_t *mem_info_get(void);

#endif
