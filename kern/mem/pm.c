/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "pm.h"

static uint8_t __bmbuff_phymm[SIZE_BITMAP_PHYMM4G] __attribute__((aligned(16)));
static spinlock_t __spinlock_phymm;
static bitmap_t __bm_phymm;

/**
 * @brief initialize physical memory system
 * 
 * @param mem_size memory size in bytes
 */
void
init_phymm_system(uint32_t mem_size) {
    spinlock_init(&__spinlock_phymm);
    bitmap_init(&__bm_phymm, mem_size >> 12, __bmbuff_phymm);
}

/**
 * @brief allocate a physical page
 * 
 * @return a physical address
 */
void *phy_alloc_page() {
    idx_t i = 0;
    wait(&__spinlock_phymm);

    i = bitmap_scan_empty(&__bm_phymm);
    bitmap_set(&__bm_phymm, i);
    signal(&__spinlock_phymm);
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

    wait(&__spinlock_phymm);
    idx_t i = ((uint32_t)page_phy_addr - MM_BASE) >> 12;
    bitmap_clear(&__bm_phymm, i);
    signal(&__spinlock_phymm);
}

/**
 * @brief get the mapping of the given virtual address
 * 
 * @param pdir the corresponding page directory table
 * @param va virtual address
 * @param pg_va the page table virtual address using when the page table missed
 * @return page table entry pointer
 */
pgelem_t *
get_mapping(pgelem_t *pdir, uint32_t va, void *pg_va) {
    if (pdir == null)    panic("get_mapping(): page directory invalid");

    pgelem_t vaddr = PGUP(va, PGSIZE);
    pgelem_t flags = pdir[PD_INDEX(vaddr)] & ~PG_MASK;
    pgelem_t *phy_pgtable = (pgelem_t *)(pdir[PD_INDEX(vaddr)] & PG_MASK);
    if (flags & PGENT_RW) {
        // how about the threads request physical pages unlimitedly?
        phy_pgtable = (pgelem_t *)phy_alloc_page();
        pgelem_t flags = PGENT_US | PGENT_RW | PGENT_PS;
        pdir[PD_INDEX(vaddr)] = ((pgelem_t)phy_pgtable) | flags;
        set_mapping(get_hoo_pgdir(), (uint32_t)pg_va, (uint32_t)phy_pgtable, flags);
    } else {
        panic("get_mapping(): invalid page table structure");
    }
    return (phy_pgtable + PT_INDEX(vaddr));
}

/**
 * @brief create page table mappings
 * 
 * @param pdir  the corresponding page directory table
 * @param va    virtual address
 * @param pa    physical address
 * @param flags flags
 */
void
set_mapping(pgelem_t *pdir, uint32_t va, uint32_t pa, pgelem_t flags) {
    if (pdir == null)    panic("set_mapping(): page directory invalid");

    __asm__ ("invlpg (%0)" : : "a" (va));
    pgelem_t *pgtbl_entry = get_mapping(pdir, va, null);
    *pgtbl_entry = ((pgelem_t)PGUP(pa, PGSIZE)) | flags;
}
