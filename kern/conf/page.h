/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_CONF_PAGE_H__
#define __KERN_CONF_PAGE_H__

#include "kern/x86.h"

#define V2P(va)             (((uint32_t)(va)) - KERN_HIGH_MAPPING)
#define PGDOWN(x, align)    (((uint32_t)x) & ~(align - 1))
#define PGUP(x, align)      (PGDOWN(((uint32_t)x + align - 1), align))

// used to calculate pg entry size
typedef uint32_t            pgelem_t;

// pg dir entry(pg table entry) amount
#define PG_STRUCT_SIZE      ((PGSIZE) / sizeof(pgelem_t))

#define MB4                 0x400000
#define PD_INDEX(x)         (((x)>>22) & 0x3ff)
#define PT_INDEX(x)         ((((uint32_t)(x))>>12) & 0x3ff)
#define PG_OFFSET(x)        ((x) & 0xfff)
#define PG_MASK             0xfffff000
#define PG(x)               (((uint32_t)(x)) & (PG_MASK))
#define PG_DIR_VA           PG_MASK
#define GET_PDE(va)         \
    (PG_DIR_VA | (PD_INDEX(PGDOWN((va), PGSIZE)) * sizeof(uint32_t)))
#define GET_PTE(va)         \
    (0xffc00000 | ((PD_INDEX(PGDOWN((va), PGSIZE)) << 12) \
    | (PT_INDEX(PGDOWN((va), PGSIZE)) * sizeof(uint32_t))))

/**
 * @brief page dir table/page table entry attribute
 */
typedef enum pg_entry_attribute {
    // present in memory
    PGENT_PS  = 1,
    // writable
    PGENT_RW  = 2,
    // user could access
    PGENT_US  = 4,
    PGENT_PWT = 8,
    PGENT_PCD = 16,
    // cpu has accessed
    PGENT_ACS = 32,
    // cpu has writing
    PGENT_D   = 64,
    PGENT_PAT = 128,
    // cpu will store to tlb
    PGENT_G   = 256
} pgent_attr_t;

#endif
