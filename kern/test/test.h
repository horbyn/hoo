/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_TEST_TEST_H__
#define __KERN_TEST_TEST_H__

#include "kern/driver/io.h"

void test_phypg_alloc(void);
void test_vspace(void);
void test_schedule(void);
void test_disk_read(void);
void test_fs(void);

#endif
