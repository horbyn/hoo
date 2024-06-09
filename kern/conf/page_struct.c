/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "page_struct.h"

/**
 * @brief setup pgstruct object
 * 
 * @param pgs      pgstruct object
 * @param pgdir_va virtual address of the page directory table
 * @param pgdir_pa physical address of the page directory table
 * @param mapping  the page table mapping
 */
void
pgstruct_set(pgstruct_t *pgs, void *pgdir_va, void *pgdir_pa, pgelem_t *mapping) {
    if (pgs == null)    panic("pgstruct_set(): null pointer");

    pgs->pdir_va_ = pgdir_va;
    pgs->pdir_pa_ = pgdir_pa;
    pgs->mapping_ = mapping;
}
