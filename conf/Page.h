/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_PAGE_H__
#define __CONF_PAGE_H__

#include "types.h"

/************************************
 * Following definition MUST be the *
 * same as `boot/kern_will_use.inc` *
 ************************************/

#define ADDR_ARDS_NUM       0x78200                         // the ARDS amount addr
#define ADDR_ARDS_BASE      0x78204                         // the ARDS itself addr

#define MM_BASE             0x100000                        // memory will be traced
#define PGSIZE              4096
#define PGDOWN(x, align)    ((x) & ~(align - 1))
#define PGUP(x, align)      (PGDOWN((x + align - 1), align))
typedef uint32_t            pgelem_t;                       // used to calculate pg entry size
#define PGDIR_SIZE          ((PGSIZE) / sizeof(pgelem_t))   // pg dir entry amount
#define PGTBL_SIZE          PGDIR_SIZE                      // pg table entry amount
#define MB4                 0x400000
#define KERN_HIGH_MAPPING   0xc0000000
#define PD_INDEX(x)         (((x)>>22) & 0x3ff)
#define PT_INDEX(x)         (((x)>>12) & 0x3ff)
#define PG_OFFSET(x)        ((x) & 0xfff)

/**
 * @brief definition of ARDS structure
 */
typedef struct {
    uint32_t base_low_;
    uint32_t base_hig_;
    uint32_t length_low_;
    uint32_t length_hig_;
    uint32_t type_;
} __attribute__((packed)) ards_t;

/**
 * @brief ARDS type enum
 */
typedef enum {
    ardstype_os = 1,                                        // OS can use
    ardstype_arch                                           // arch reserves
} ardstype_t;

/**
 * @brief node used for linking all memory
 */
typedef struct phy_page {
    uint8_t *pgaddr;                                        // page address
    struct phy_page *next;                                  // indicate the next node
} ppg_t;

/**
 * @brief physical page range
 */
typedef struct phy_page_range {
    uint8_t *ppg_base;                                      // the first page
    uint8_t *ppg_end;                                       // the last page
    size_t pg_amount;                                       // pages amount
} ppg_range_t;

/**
 * @brief page dir table/page table entry attribute
 */
typedef enum pg_entry_attribute {
    PGENT_PS  = 1,                                          // present in memory
    PGENT_RW  = 2,                                          // writable
    PGENT_US  = 4,                                          // user could access
    PGENT_PWT = 8,
    PGENT_PCD = 16,
    PGENT_ACS = 32,                                         // cpu has accessed
    PGENT_D   = 64,                                         // cpu has writing
    PGENT_PAT = 128,
    PGENT_G   = 256                                         // cpu will store to tlb
} pgent_attr_t;

#endif
