/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "mm.h"

__attribute__ ((section(".init.data")))
    static pgelem_t page_dir[PGDIR_SIZE];
__attribute__ ((section(".init.data")))
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
__attribute__ ((section(".init.text"))) void
setup_vmm(void) {
    // setup low 4mb mapping
    create_pgtbl_map(page_tbl_4mb, 0, 0, MB4 / PGSIZE);

    // setup kernel page dir table
    create_ptdir_map(page_dir, 0, page_tbl_4mb);            // #0
    create_ptdir_map(page_dir, PD_INDEX(KERN_HIGH_MAPPING),
        page_tbl_4mb);                                      // #768
    create_ptdir_map(page_dir, PD_INDEX(0xffc00000),
        page_dir);                                          // #1023

    // paging
    paging(page_dir);
}
