/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEM_MMU_H__
#define __KERN_MEM_MMU_H__

#include "page.h"
#include "preconf.h"
#include "paddr.h"
#include "kern/x86.h"
#include "kern/debug.h"

void mapping(pgelem_t *pdir, uint32_t va, uint32_t pa);

#endif
