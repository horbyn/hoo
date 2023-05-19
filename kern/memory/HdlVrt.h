/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MEMORY_HDLVRT_H__
#define __KERN_MEMORY_HDLVRT_H__

#include "types.h"
#include "x86.h"
#include "lib/lib.h"

typedef uint32_t    pgelem_t;                               // used to calculate pg entry size
#define PGDIR_SIZE  ((PGSIZE) / sizeof(pgelem_t))           // pg dir entry amount
#define PGTBL_SIZE  PGDIR_SIZE                              // pg table entry amount

void create_pgtbl_map(void *pgt, size_t ent_base, void *
    pg_phy_addr, size_t n);
void create_ptdir_map(void *pgd, size_t ent, void *pgt);

#endif
