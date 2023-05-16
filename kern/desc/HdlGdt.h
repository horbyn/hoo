/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_DESC_HDLGDT_H__
#define __KERN_DESC_HDLGDT_H__

#include "StuffGdt.h"
#include "lib/lib.h"

#define SIZE_GDT    8                                       // the gdt descriptors amount
extern Gdt_t  __gdt[SIZE_GDT];
extern Gdtr_t __gdtr;

void set_gdt(size_t idx, uint32_t limit, uint32_t base,
    uint8_t a, uint8_t rw, uint8_t dc, uint8_t exe,
    uint8_t sys, uint8_t dpl, uint8_t ps, uint8_t l,
    uint8_t db, uint8_t g);
void set_gdtr();

#endif
