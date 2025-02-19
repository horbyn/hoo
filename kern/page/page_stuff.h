#pragma once
#ifndef __KERN_PAGE_PAGE_STUFF_H__
#define __KERN_PAGE_PAGE_STUFF_H__

#include "boot/config_boot.h"
#include "user/types.h"

#define PGFLAG_PS           1
#define PGFLAG_RW           2
#define PGFLAG_US           4

#define V2P(va)             (((uint32_t)(va)) - KERN_HIGH_MAPPING)
#define PGDOWN(x, align)    (((uint32_t)(x)) & ~((align) - 1))
#define PGUP(x, align)      (PGDOWN(((uint32_t)(x) + (align) - 1), (align)))
#define PD_INDEX(x)         (((x)>>22) & 0x3ff)
#define PT_INDEX(x)         ((((uint32_t)(x))>>12) & 0x3ff)

// 用来计算 paging-structure 的大小
typedef uint32_t            pgelem_t;

#define MB4                 0x400000
#define PGSIZE              4096
// 一个页目录表（页表）内有多少个 paging-structure
#define PG_STRUCT_SIZE      ((PGSIZE) / sizeof(pgelem_t))
#define PG_MASK             0xfffff000
#define PG(x)               (((uint32_t)(x)) & (PG_MASK))
#define PG_DIR_VA           PG_MASK
#define GET_PDE(va)         \
    (PG_DIR_VA | (PD_INDEX(PGDOWN((va), PGSIZE)) * sizeof(uint32_t)))
#define GET_PTE(va)         \
    (0xffc00000 | ((PD_INDEX(PGDOWN((va), PGSIZE)) << 12) \
    | (PT_INDEX(PGDOWN((va), PGSIZE)) * sizeof(uint32_t))))

#endif
