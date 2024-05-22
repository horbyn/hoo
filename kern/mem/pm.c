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
 * @param pgs   the paging structure
 * @param va    virtual address
 * @return page table entry pointer
 */
pgelem_t *
get_mapping(pgstruct_t *pgs, uint32_t va) {
    if (pgs == null)    panic("get_mapping(): null pointer");

    pgelem_t vaddr = PGUP(va, PGSIZE);
    pgelem_t flags = *((pgelem_t *)pgs->pdir_va_ + PD_INDEX(vaddr)) & ~PG_MASK,
        FLAGS = PGENT_US | PGENT_RW | PGENT_PS;
    if ((flags & FLAGS) != FLAGS)
        panic("get_mapping(): invalid page table structure");
    return (pgelem_t *)(pgs->mapping_[PD_INDEX(vaddr)]) + PT_INDEX(vaddr);
}

/**
 * @brief create page table mappings
 * 
 * @param pgs   the paging structure
 * @param va    virtual address
 * @param pa    physical address
 * @param flags flags
 */
void
set_mapping(pgstruct_t *pgs, uint32_t va, uint32_t pa, pgelem_t flags) {
    if (pgs == null)    panic("set_mapping(): null pointer");

    __asm__ ("invlpg (%0)" : : "a" (va));
    pgelem_t *pte = get_mapping(pgs, va);
    *pte = ((pgelem_t)PGUP(pa, PGSIZE)) | flags;
}
