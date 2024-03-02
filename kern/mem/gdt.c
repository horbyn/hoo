/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "gdt.h"

/**
 * @brief Set the gdt descriptor
 * 
 * @param d     the gdt entry
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
set_gdt(Desc_t *d, uint32_t limit, uint32_t base, uint8_t a,
uint8_t rw, uint8_t dc, uint8_t exe, uint8_t sys, uint8_t dpl,
uint8_t ps, uint8_t l, uint8_t db, uint8_t g) {
    if (!d)    hlt();

    d->limit_15_0_                          = (uint16_t)limit;
    d->limit_19_16_                         = (uint8_t)(limit >> 16);
    d->base_15_0_                           = (uint16_t)base;
    d->base_23_16_                          = (uint8_t)(base >> 16);
    d->base_31_24_                          = (uint8_t)(base >> 24);
    if (sys != 0) {
        // not system segment
        d->access_bytes_.code_or_data_.a_   = a;
        d->access_bytes_.code_or_data_.rw_  = rw;
        d->access_bytes_.code_or_data_.dc_  = dc;
        d->access_bytes_.code_or_data_.e_   = exe;
        d->access_bytes_.code_or_data_.sys_ = 1;
        d->access_bytes_.code_or_data_.dpl_ = dpl;
        d->access_bytes_.code_or_data_.ps_  = ps;
    } else {
        // system segment
        d->access_bytes_.tss_.type1_        = 1;
        d->access_bytes_.tss_.type2_        = rw;
        d->access_bytes_.tss_.type3_        = 0;
        d->access_bytes_.tss_.type4_        = 1;
        d->access_bytes_.tss_.sys_          = 0;
        d->access_bytes_.tss_.dpl_          = dpl;
        d->access_bytes_.tss_.ps_           = ps;
    }
    d->avl_                                 = 0;
    d->long_                                = l;
    d->db_                                  = db;
    d->g_                                   = g;
}

/**
 * @brief Set the gdtr object
 * 
 * @param gdtr     the gdtr object
 * @param gdt      the gdt
 * @param gdt_size the gdt length
 */
void set_gdtr(Gdtr_t *gdtr, Desc_t *gdt, uint32_t gdt_size) {
    if (!gdtr)    hlt();

    gdtr->size_   = gdt_size;
    gdtr->linear_ = gdt;
    __asm__ ("lgdt (%0)" : :"r"(gdtr));
}
