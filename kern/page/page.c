#include "page.h"

/**
 * @brief 开启分页
 */
void
paging() {
    pgelem_t flags = PGFLAG_US | PGFLAG_RW | PGFLAG_PS;

    pgelem_t *pdir = (pgelem_t *)PGDIR_HOO;
    pdir[0] = (pgelem_t)((SEG_PGTABLE * 16) | flags);
    pdir[PD_INDEX(KERN_HIGH_MAPPING)] = pdir[0];
    pdir[PG_STRUCT_SIZE - 1] = (pgelem_t)(V2P(pdir) | flags);

    pgelem_t *pg = (pgelem_t *)(SEG_PGTABLE * 16);
    for (uint32_t i = 0; i < (MM_BASE / PGSIZE); ++i)
        pg[i] = ((pgelem_t)(i * PGSIZE) | flags);

    // Paging and Write-Protect (使用 C.O.W 要提前开启)
    __asm__ ("movl %0, %%cr3\r\n"
        "movl %%cr0,       %%eax\r\n"
        "orl $0x80010000,  %%eax\r\n"
        "movl %%eax,       %%cr0" : :"r"(V2P(pdir)));
}
