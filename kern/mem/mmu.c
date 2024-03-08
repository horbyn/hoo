/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "mmu.h"

/**
 * @brief create page table mappings
 * 
 * @param pdir the corresponding page directory table
 * @param va   virtual address
 * @param pa   physical address
 */
void
mapping(pgelem_t *pdir, uint32_t va, uint32_t pa) {
    if (pdir == null)    panic("mapping(): page directory invalid");

    uint32_t flags = (uint32_t)PGENT_US | PGENT_RW | PGENT_PS;
    pgelem_t vaddr = PGUP(va, PGSIZE);
    bool is_inmem = pdir[PD_INDEX(vaddr)] & PGENT_PS;
    pgelem_t *phy_pgtable = (pgelem_t *)(pdir[PD_INDEX(vaddr)] & (~flags));
    if (!is_inmem) {
        // how about the threads request physical pages unlimitedly?
        phy_pgtable = (pgelem_t *)phy_alloc_page();
        pdir[PD_INDEX(vaddr)] = ((pgelem_t)phy_pgtable) | flags;
    }
    phy_pgtable[PT_INDEX(vaddr)] =
        ((pgelem_t)PGUP(pa, PGSIZE)) | flags;
}
