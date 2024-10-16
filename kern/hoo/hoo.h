/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_HOO_HOO_H__
#define __KERN_HOO_HOO_H__

#include "kern/desc/desc.h"
#include "kern/page/page_stuff.h"

#define PGDIR_HOO   ((SEG_PDTABLE) * 16 + (KERN_HIGH_MAPPING))

tss_t *get_hoo_tss(void);

#endif
