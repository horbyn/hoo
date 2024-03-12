/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "mm.h"

static spinlock_t __spinlock_phymm;
static bitmap_t __bm_phymm;
static uint8_t __bmbuff_phymm[SIZE_BITMAP_PHYMM4G] __attribute__((aligned(4)));
static mminfo_t __mminfo = { MM_BASE, 0 };

/**
 * @brief physical memory initialization
 */
void
mem_info_init() {

    // traversal ARDS to find out the available mm.
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (uint32_t i = 0; i < *ards_num; ++i) {
        if ((ards[i].type_ == ards_type_os)
        && (ards[i].base_low_ == __mminfo.base_ + __mminfo.length_)) {
            // record only continuous memory space
            __mminfo.length_ += ards[i].length_low_;
        }
    }

    if (__mminfo.length_ == 0)
        panic("mem_info_init(): cannot get memory info");
    else
        kprintf("================ MEMORY  INFO ================\n"
                "begin: 0x%x, end: 0x%x\n", __mminfo.base_,
                __mminfo.base_ + __mminfo.length_);

    spinlock_init(&__spinlock_phymm);
    bitmap_init(&__bm_phymm, __mminfo.length_ >> 12, __bmbuff_phymm);
}

/**
 * @brief get the system memory info
 * 
 * @return memory info structure
 */
const mminfo_t *
mem_info_get(void) {
    return &__mminfo;
}

/**
 * @brief allocate a physical page
 * 
 * @return a physical address
 */
void *
phy_alloc_page() {
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
    wait(&__spinlock_phymm);
    idx_t i = ((uint32_t)page_phy_addr - MM_BASE) >> 12;
    bitmap_clear(&__bm_phymm, i);
    signal(&__spinlock_phymm);
}

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
        if (((uint32_t)phy_pgtable | 0xfffff000) != 0xfffff000)
            panic("mapping(): the page dir table address is unaligned");
        pdir[PD_INDEX(vaddr)] = ((pgelem_t)phy_pgtable) | flags;
    }
    phy_pgtable[PT_INDEX(vaddr)] =
        ((pgelem_t)PGUP(pa, PGSIZE)) | flags;
}
