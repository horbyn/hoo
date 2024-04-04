/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_INTR_STUFF_INTR_H__
#define __KERN_INTR_STUFF_INTR_H__

#include "kern/x86.h"

#define IDT_ENTRIES_NUM     256

typedef void (*isr_t)(void);

/**
 * @brief idt descriptor definition
 */
typedef struct {
    /*
     * 63           48 47 46 45 44 43     40 39    32
     * ┌──────────────┼──┼─────┼──┼─────────┼────────┐
     * │Offset 31..16 │P │ DPL │ 0│Gate Type│Reserved│
     * └──────────────┴──┴─────┴──┴─────────┴────────┘
     * 31               16 15                       0
     * ┌──────────────────┼──────────────────────────┐
     * │Segment Selector  │  Offset 15..0            │
     * └──────────────────┴──────────────────────────┘
     */

    // will be loaded to EIP
    uint16_t    isr_low_;
    // will be loaded to CS when interrupt happened
    uint16_t    selector_;
    uint8_t     reserved_;
    uint8_t     attributes_;
    // will be loaded to EIP
    uint16_t    isr_high_;
} __attribute__((packed)) idt_t;

/**
 * @brief idtr structure
 */
typedef struct {
    uint16_t	limit_;
    uint32_t	base_;
} __attribute__((packed)) idtr_t;

/**
 * @brief enum of privilege level
 */
typedef enum privilege_level {
    PL_KERN = 0,
    PL_USER = 3
} privilege_t;

/**
 * @brief enum of gate descriptor
 */
typedef enum gate_descriptor {
    INTER_GATE = 0x0e,
    TRAP_GATE = 0x0f
} gatedesc_t;

#endif
