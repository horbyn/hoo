/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "intr.h"

/**
 * @brief setup the idt entry
 * 
 * @param idt  idt object
 * @param dpl  descriptor privilege level
 * @param gate gate descriptor
 * @param addr routine entry
 */
void
set_idt_entry(idt_t *idt, privilege_t dpl, gatedesc_t gate, uint32_t addr) {
    if (idt == null)    panic("set_idt_entry(): null pointer");

    // the high 4 bytes are `P/DPL/0`
    uint8_t attr = (1 << 7 | dpl << 5 | (uint8_t)gate);

    idt->isr_low_    = (uint16_t)addr;
    idt->selector_   = CS_SELECTOR_KERN;
    idt->reserved_   = 0;
    idt->attributes_ = attr;
    idt->isr_high_   = (uint16_t)(addr >> 16);
}

/**
 * @brief setup the isr entry
 * 
 * @param isr  isr object
 * @param addr routine entry
 */
void
set_isr_entry(isr_t *isr, isr_t addr) {
    if (isr == null)    panic("set_isr_entry(): null pointer");
    *isr = addr;
}
