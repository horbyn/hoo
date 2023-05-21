/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "HdlVrt.h"

/**
 * @brief setup page table mapping
 * 
 * @param pgt               page table addr
 * @param ent_base          page table entry base
 * @param pg_phy_addr_base  the physical address base of the page
 * @param n                 the amount needed to setup
 */
void
create_pgtbl_map(void *pgt, size_t ent_base, void *
pg_phy_addr_base, size_t n) {
    // DONOT handle the following condition temporary
    ASSERT(pgt == null);
    ASSERT(ent_base < 0 || ent_base > PGDIR_SIZE);
    ASSERT(n > PGTBL_SIZE);

    if (n == 0)    return;

    // adjust the amount to setup
    n = (ent_base + n) > PGTBL_SIZE ?
        PGTBL_SIZE - ent_base : n;

    bzero(pgt, PGSIZE);
    pgelem_t *worker = (pgelem_t *)pgt;
    uint32_t pg =
        (uint32_t)PGUP((uint32_t)pg_phy_addr_base, PGSIZE);

    for (size_t i = 0; i < n; ++i, pg += PGSIZE)
        worker[ent_base + i] =
            (pg | PGENT_US | PGENT_RW | PGENT_PS);
}

/**
 * @brief setup page dir mapping
 * 
 * @param pgd page dir addr
 * @param ent page dir entry
 * @param pgt page table physical addr
 */
void
create_ptdir_map(void *pgd, size_t ent, void *pgt) {
    ASSERT(pgd == null);
    ASSERT(ent < 0 || ent > PGDIR_SIZE);

    if (pgt == null)    return;

    pgelem_t *worker = (pgelem_t *)pgd;
    worker[ent] =
        ((uint32_t)pgt | PGENT_US | PGENT_RW | PGENT_PS);
}
