/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_8253_H__
#define __DEVICE_8253_H__

#include "Cmd8253.h"
#include "kern/x86.h"
#include "kern/types.h"

void set_command(sc_t, am_t, om_t, bm_t);
void set_counter(size_t);

#endif
