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
#define PGDOWN(x, align)    ((x) & ~(align - 1))
#define PGUP(x, align)      (PGDOWN((x + align - 1), align))

// used to calculate pg entry size
typedef uint32_t            pgelem_t;

// pg dir entry amount
#define PGDIR_SIZE          ((PGSIZE) / sizeof(pgelem_t))
// pg table entry amount
#define PGTBL_SIZE          PGDIR_SIZE

#define MB4                 0x400000
#define PD_INDEX(x)         (((x)>>22) & 0x3ff)
#define PT_INDEX(x)         (((x)>>12) & 0x3ff)
#define PG_OFFSET(x)        ((x) & 0xfff)

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
