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

/**
 * @brief initialize the hoo thread
 */
void
init_hoo(void) {

    pcb_t *hoo_pcb = get_hoo_pcb();

    /*
     * The kernel linear space as following:
     * kernel space: all common threads share
     * kernel dynamic space: dynamically allocate memory by kernel itself
     * kernel metadata space: kernel metadata
     *
     *                            kernel space    dynamic space
     * +----------+---------------+---------------+-------------------------+
     * |kernel elf|               |elf mapping    | dynamic   ..  metadata  |
     * +----------+---------------+---------------+-------------------------+
     *            0x00xx_x000     0x8000_0000     0x80xx_x000     0xc000_0000
     */

    // the dynamic space begins at 0x0040_0000 (as same as 0x8040_0000)
    //   because our kernel is very small that smaller than 1 page
    //   directory table
    hoo_pcb->break_ = MB4;

    // initialize some metadata
    init_thread_buckmngr(hoo_pcb);
    init_thread_fmngr(hoo_pcb);

    // The executable flow as far from boot to there,
    // uses the boot stack. Now we call this flow to
    // hoo thread, and the stack it used is hoo stack
    pcb_set(hoo_pcb, (uint32_t *)STACK_HOO_RING0, (uint32_t *)STACK_HOO_RING3,
        TID_HOO, (pgelem_t *)(V2P(get_hoo_pgdir())), &hoo_pcb->vmngr_, TIMETICKS,
        thread_buckmngr_get(TID_HOO), thread_fmngr_get(TID_HOO), hoo_pcb->break_);

}
