/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_CONF_PAGE_STRUCT_H__
#define __KERN_CONF_PAGE_STRUCT_H__

#include "page.h"
#include "kern/driver/io.h"

/**
 * @brief paging struct
 */
typedef struct paging_struct {
    // virtual address of this thread page directory table
    void *pdir_va_;
    // physical address of this thread page directory table
    void *pdir_pa_;
    // a 4KB buffer to record all virtual address of the page tables
    pgelem_t *mapping_;
} __attribute__((packed)) pgstruct_t;

void  pgstruct_set(pgstruct_t *pgs, void *pgdir_va, void *pgdir_pa,
    pgelem_t *mapping);

#endif
