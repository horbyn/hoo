/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "pic.h"

/**
 * @brief pic initialization
 */
void
init_pic(void) {
    set_icw1(ICW1_ICW4);                                    // level; cascade; icw4
    set_icw2(0x20, 0x28);
    set_icw3(2);
    set_icw4(ICW4_AUTO);                                    // CFNM; no-buffer; auto EOI

    set_mask_ocw1(IRQ_TIMER);                               // irq#0
    set_mask_ocw1(IRQ_KEYBOARD);                            // irq#1
    clear_mask_ocw1(IRQ_CASCADE);                           // irq#2
    set_mask_ocw1(IRQ_COM2);                                // irq#3
    set_mask_ocw1(IRQ_COM1);                                // irq#4
    set_mask_ocw1(IRQ_LPT2);                                // irq#5
    clear_mask_ocw1(IRQ_FLOPPY);                            // irq#6
    set_mask_ocw1(IRQ_LPT1);                                // irq#7
    set_mask_ocw1(IRQ_CMOS_RTC);                            // irq#8
    set_mask_ocw1(IRQ_ACPI);                                // irq#9
    set_mask_ocw1(IRQ_OPEN_SCSI_NIC1);                      // irq#10
    set_mask_ocw1(IRQ_OPEN_SCSI_NIC2);                      // irq#11
    set_mask_ocw1(IRQ_MOUSE);                               // irq#12
    set_mask_ocw1(IRQ_MATH);                                // irq#13
    set_mask_ocw1(IRQ_ATA1);                                // irq#14
    set_mask_ocw1(IRQ_ATA2);                                // irq#15
}
