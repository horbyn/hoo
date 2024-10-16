/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "hoo.h"

/**
 * @brief get the page directory table of hoo thread
 * 
 * @return the page directory
 */
pgelem_t *
get_hoo_pgdir(void) {
    return (pgelem_t *)(SEG_PDTABLE * 16 + KERN_HIGH_MAPPING);
}

/**
 * @brief get the tss object of hoo thread
 * @note this tss object is only one
 * @return tss object
 */
tss_t *
get_hoo_tss(void) {
    static tss_t tss;
    return &tss;
}

/**
 * @brief get the pcb of hoo thread
 * 
 * @return the pcb
 */
pcb_t *
get_hoo_pcb(void) {
    // the hoo thread uses pcb by statically allocation
    //   while the rest by dynamically allocation
    static pcb_t hoo_pcb;
    return &hoo_pcb;
}
