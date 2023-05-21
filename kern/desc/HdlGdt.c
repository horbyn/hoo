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

    __gdt[idx].limit_15_0_        = (uint16_t)limit;
    __gdt[idx].limit_19_16_       = (uint8_t)(limit >> 16);
    __gdt[idx].base_15_0_         = (uint16_t)base;
    __gdt[idx].base_23_16_        = (uint8_t)(base >> 16);
    __gdt[idx].base_31_24_        = (uint8_t)(base >> 24);
    __gdt[idx].access_bytes_.a_   = a;
    __gdt[idx].access_bytes_.rw_  = rw;
    __gdt[idx].access_bytes_.dc_  = dc;
    __gdt[idx].access_bytes_.e_   = exe;
    __gdt[idx].access_bytes_.sys_ = sys;
    __gdt[idx].access_bytes_.dpl_ = dpl;
    __gdt[idx].access_bytes_.ps_  = ps;
    __gdt[idx].rsv_               = 0;
    __gdt[idx].long_              = l;
    __gdt[idx].db_                = db;
    __gdt[idx].g_                 = g;
}

/**
 * @brief Set the gdtr object
 */
void set_gdtr() {
    __gdtr.size_   = sizeof(__gdt);
    __gdtr.linear_ = __gdt;
    load_gdtr(&__gdtr);
}
