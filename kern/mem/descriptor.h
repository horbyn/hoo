/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __CONF_GDT_H__
#define __CONF_GDT_H__

#include "kern/types.h"

#define CS_SELECTOR_KERN    0x08
#define DS_SELECTOR_KERN    0x10
#define CS_SELECTOR_USER    0x1b                            // 01_1011
#define DS_SELECTOR_USER    0x23                            // 10_0011

/**
 * @brief definition of access byte field of those
 * descriptor not system segment
 */
typedef struct AccessByteNotsys {
    /*
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤
     * │P│DPL │S│E│DC│RW│A│
     * └─┴────┴─┴─┴──┴──┴─┘
     */

    uint8_t a_   :1;                                        // Access: best left clear(0)
    uint8_t rw_  :1;                                        // readable or writable
    uint8_t dc_  :1;                                        // direction or conforming
    uint8_t e_   :1;                                        // executable
    uint8_t sys_ :1;                                        // sys segment: clear if it is
    uint8_t dpl_ :2;                                        // privilege
    uint8_t ps_  :1;                                        // present: set if in memory
    
} __attribute__((packed)) AcsNotsys_t;

/**
 * @brief definition of access byte field in tss
 * which is belong to system segment
 */
typedef struct AccessByteTss {
    /*
     * used for TSS        
     * ┌─┬──┬─┬─┬─┬─┬─┬─┐
     * │7│ 6│5│4│3│2│1│0│
     * ├─┼──┴─┼─┼─┼─┼─┼─┤
     * │P│DPL │S│1│0│B│1│
     * └─┴────┴─┴─┴─┴─┴─┘
     */

    uint8_t type1_ :1;                                      // fixed with 1
    uint8_t type2_ :1;                                      // busy: set if the task is executing
    uint8_t type3_ :1;                                      // fixed with 0
    uint8_t type4_ :1;                                      // fixed with 1
    uint8_t sys_   :1;                                      // sys segment: clear if it is
    uint8_t dpl_   :2;                                      // privilege
    uint8_t ps_    :1;                                      // present: set if in memory
    
} __attribute__ ((packed)) AcsTss_t;

/**
 * @brief gdt definition
 */
typedef struct Descriptor {
    uint16_t  limit_15_0_;
    uint16_t  base_15_0_;
    uint8_t   base_23_16_;
    /*
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤
     * │P│DPL │S│E│DC│RW│A│
     * └─┴────┴─┴─┴──┴──┴─┘
     */

    union {
        AcsNotsys_t  code_or_data_;
        AcsTss_t     tss_;
    } access_bytes_;
    uint8_t      limit_19_16_ :4;
    /*
     * ┌─┬──┬─┬────────┐
     * │3│ 2│1│       0│
     * ├─┼──┼─┼────────┤
     * │G│DB│L│Reserved│
     * └─┴──┴─┴────────┘
     */

    uint8_t   avl_  : 1;                                    // avilable for use by os(here is not so always clear)
    uint8_t   long_ : 1;                                    // long mode if set
    uint8_t   db_   : 1;                                    // protected mode
    uint8_t   g_    : 1;                                    // granularity
    uint8_t   base_31_24_;
} __attribute__ ((packed)) Desc_t;

/**
 * @brief definition of GDTR
 */
typedef struct GdtRegister {
    uint16_t size_;                                         // gdt size
    Desc_t *linear_;                                        // gdt linear base
} __attribute__ ((packed)) Gdtr_t;

#endif
