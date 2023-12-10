/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_CONFIG_H__
#define __CONF_CONFIG_H__

#include "Descriptor.h"
#include "Page.h"
#include "preconf.h"
#include "Tss.h"
#include "kern/debug.h"

#define SIZE_GDT    8                                       // the gdt descriptors amount

extern Tss_t __tss;

void kernel_config(void);
void config_gdt(void);
void config_tss(void);
void config_free_list(void);
void config_paging(void);
void set_gdt(size_t idx, uint32_t limit, uint32_t base,
    uint8_t a, uint8_t rw, uint8_t dc, uint8_t exe,
    uint8_t sys, uint8_t dpl, uint8_t ps, uint8_t l,
    uint8_t db, uint8_t g);
void set_gdtr(void);
void create_pgtbl_map(void *pgt, size_t ent_base, void *
    pg_phy_addr, size_t n);
void create_ptdir_map(void *pgd, size_t ent, void *pgt);

#endif
