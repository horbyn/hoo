/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "pm.h"

static uint8_t __bmbuff_phymm[SIZE_BITMAP_PHYMM4G] __attribute__((aligned(16)));
static bitmap_t __bm_phymm;

/**
 * @brief initialize physical memory system
 * 
 * @param mem_size memory size in bytes
 */
void
init_phymm_system(uint32_t mem_size) {
    bitmap_init(&__bm_phymm, mem_size >> 12, __bmbuff_phymm);
}

/**
 * @brief allocate a physical page
 * 
 * @return a physical address
 */
void *phy_alloc_page() {
    int i = 0;

    i = bitmap_scan_empty(&__bm_phymm);
    bitmap_set(&__bm_phymm, i);
    return (void *)((i <<= 12) + MM_BASE);
}

/**
 * @brief release a physical page
 * 
 * @param page_phy_addr a physical address
 */
void
phy_release_page(void *page_phy_addr) {
    if (page_phy_addr == null)    return;
    if ((uint32_t)page_phy_addr < MM_BASE)
        panic("phy_release_page(): cannot release kernel physical memory");

    int i = ((uint32_t)page_phy_addr - MM_BASE) >> 12;
    bitmap_clear(&__bm_phymm, i);
}

/**
 * @brief create page table mappings
 * 
 * @param va    virtual address
 * @param pa    physical address
 * @param flags flags
 */
void
set_mapping(void *va, void *pa, pgelem_t flags) {

    __asm__ ("invlpg (%0)" : : "a" (va));
    pgelem_t *pgdir_va = (pgelem_t *)GET_PDE(va);
    const pgelem_t FLAGS = PGFLAG_US | PGFLAG_RW | PGFLAG_PS,
        FLAGS_PF = PGFLAG_US | PGFLAG_PS;
    pgelem_t pde_flags = (*pgdir_va & ~PG_MASK) & FLAGS;
    if (pde_flags != FLAGS && pde_flags != FLAGS_PF) {
        // lack of page table
        void *pgtbl = phy_alloc_page();
        *pgdir_va = (pgelem_t)pgtbl | FLAGS;
    }
    pgelem_t *pgtbl_va = (pgelem_t *)GET_PTE(va);
    *pgtbl_va = (pgelem_t)pa | flags;
}
