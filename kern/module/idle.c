/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "idle.h"

/**
 * @brief get the page directory table of idle thread
 * 
 * @return the page directory
 */
pgelem_t *
get_idle_pgdir(void) {
    return (pgelem_t *)(SEG_PDTABLE * 16 + KERN_HIGH_MAPPING);
}

/**
 * @brief get the tss object of idle thread
 * @note this tss object is only one
 * @return tss object
 */
tss_t *
get_idle_tss(void) {
    static tss_t tss;
    return &tss;
}

/**
 * @brief get the pcb of idle thread
 * 
 * @return the pcb
 */
pcb_t *
get_idle_pcb(void) {
    return (pcb_t *)STACK_IDLE_RING3;
}
