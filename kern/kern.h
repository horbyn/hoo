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
#include "module/do_intr.h"
#include "module/driver.h"
#include "module/fs.h"
#include "module/loader.h"
#include "module/first.h"
#include "module/mem.h"
#include "module/sched.h"
#include "driver/io.h"

extern uint8_t __kern_base[], __kern_end[];

void kernel_init(void);
void kernel_exec(void);
void kernel_loop(void);

#endif
