/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DESC_GDT_H__
#define __KERN_DESC_GDT_H__

#include "desc.h"

void set_gdt(Desc_t *d, uint32_t limit, uint32_t base,uint8_t a, uint8_t rw,
    uint8_t dc, uint8_t exe, uint8_t sys, uint8_t dpl, uint8_t ps, uint8_t l,
    uint8_t db, uint8_t g);
void set_gdtr(Gdtr_t *gdtr, Desc_t *gdt, uint32_t gdt_size);

#endif
