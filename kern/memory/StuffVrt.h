/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEMORY_STUFFVRT_H__
#define __KERN_MEMORY_STUFFVRT_H__

#include "types.h"

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
