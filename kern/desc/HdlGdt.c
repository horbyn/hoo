/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "HdlGdt.h"

Gdt_t  __gdt[SIZE_GDT];
Gdtr_t __gdtr;

/**
 * @brief Set the gdt descriptor
 * 
 * @param idx   the table idx(if beyond then nothing happen)
 * @param limit the maximun of addressing, which units are either 1B or 4KB
 * @param base  segment base linear address
 * @param a     best left clear
 * @param rw    readable or writable
 * @param dc    direction or conforming
 * @param exe   executable
 * @param sys   if system segment?
 * @param dpl   privilege
 * @param ps    if present in memory?
 * @param l     if the long mode?
 * @param db    if the protected mode?
 * @param g     granularity
 */
void
set_gdt(size_t idx, uint32_t limit, uint32_t base, uint8_t a,
uint8_t rw, uint8_t dc, uint8_t exe, uint8_t sys, uint8_t dpl,
uint8_t ps, uint8_t l, uint8_t db, uint8_t g) {
    ASSERT(idx >= SIZE_GDT);

    Gdt_t desc;
    desc.limit_15_0_        = (uint16_t)limit;
    desc.limit_19_16_       = (uint8_t)limit >> 16;
    desc.base_15_0_         = (uint16_t)base;
    desc.base_23_16_        = (uint8_t)base >> 16;
    desc.base_31_24_        = (uint8_t)base >> 24;
    desc.access_bytes_.a_   = a;
    desc.access_bytes_.rw_  = rw;
    desc.access_bytes_.dc_  = dc;
    desc.access_bytes_.e_   = exe;
    desc.access_bytes_.sys_ = sys;
    desc.access_bytes_.dpl_ = dpl;
    desc.access_bytes_.ps_  = ps;
    desc.rsv_               = 0;
    desc.long_              = l;
    desc.db_                = db;
    desc.g_                 = g;
    __gdt[idx] = desc;
}

/**
 * @brief Set the gdtr object
 */
void set_gdtr() {
    __gdtr.size_   = NELEMS(__gdt);
    __gdtr.linear_ = __gdt;
    load_gdtr(&__gdtr);
}
