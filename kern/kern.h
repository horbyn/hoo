/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_KERN_H__
#define __KERN_KERN_H__

#include "module/config.h"
#include "module/mem.h"
#include "module/sched.h"
#include "driver/io.h"
#ifdef TEST
    #include "test/test.h"
#endif

extern uint8_t __kern_base[], __kern_end[];

void kernel_init(void);

#endif
