/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "mm.h"

static pgelem_t page_dir[PGDIR_SIZE];
static pgelem_t page_tbl_4mb[PGDIR_SIZE];                   // only the low 4mb map

/**
 * @brief setup physical memory
 */
void
setup_pmm(void) {
    init_phymm();
}

/**
 * @brief setup virtual memory
 */
void
setup_vmm(void) {
    // setup low 4mb mapping
    create_pgtbl_map(page_tbl_4mb, 0, 0, MB4 / PGSIZE);

    // setup kernel page dir table
    create_ptdir_map(page_dir, 0, page_tbl_4mb);            // #0
    size_t page_tbl_represent = PGTBL_SIZE * PGSIZE;        // 4mb
    size_t high_ent =
        KERN_HIGH_MAPPING / page_tbl_represent;
    create_ptdir_map(page_dir, high_ent, page_tbl_4mb);     // #768

    // paging
    paging(page_dir);
}
