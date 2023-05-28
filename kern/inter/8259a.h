/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_INTER_8259A_H__
#define __KERN_INTER_8259A_H__

#include "types.h"
#include "Cmd8259a.h"
#include "x86.h"

void set_icw1(uint8_t);
void set_icw2(uint8_t master, uint8_t slave);
void set_icw3(uint8_t);
void set_icw4(uint8_t);
void set_mask_ocw1(uint8_t);
void clear_mask_ocw1(uint8_t);

#endif
