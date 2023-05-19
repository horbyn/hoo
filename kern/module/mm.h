/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MODULE_MM_H__
#define __KERN_MODULE_MM_H__

#include "memory/HdlPhy.h"
#include "memory/HdlVrt.h"

#define MB4     0x400000

void setup_pmm(void);
void setup_vmm(void);

#endif
