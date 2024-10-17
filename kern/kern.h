/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_KERN_H__
#define __KERN_KERN_H__

#include "module/conf.h"
#include "module/do_intr.h"
#include "module/driver.h"
#include "module/mem.h"

void kern_init();
void kern_exec();

#endif
