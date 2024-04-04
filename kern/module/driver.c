/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "driver.h"

/**
 * @brief pic initialization
 */
static void
init_pic(void) {
    set_icw1(ICW1_ICW4);                                    // level; cascade; icw4
    set_icw2(0x20, 0x28);
    set_icw3(2);
    set_icw4(ICW4_AUTO);                                    // CFNM; no-buffer; normal EOI

    enable_mask_ocw1(IRQ_TIMER);                            // irq#0
    disable_mask_ocw1(IRQ_KEYBOARD);                        // irq#1
    enable_mask_ocw1(IRQ_CASCADE);                          // irq#2
    disable_mask_ocw1(IRQ_COM2);                            // irq#3
    disable_mask_ocw1(IRQ_COM1);                            // irq#4
    disable_mask_ocw1(IRQ_LPT2);                            // irq#5
    disable_mask_ocw1(IRQ_FLOPPY);                          // irq#6
    disable_mask_ocw1(IRQ_LPT1);                            // irq#7
    disable_mask_ocw1(IRQ_CMOS_RTC);                        // irq#8
    disable_mask_ocw1(IRQ_ACPI);                            // irq#9
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC1);                  // irq#10
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC2);                  // irq#11
    disable_mask_ocw1(IRQ_MOUSE);                           // irq#12
    disable_mask_ocw1(IRQ_MATH);                            // irq#13
    disable_mask_ocw1(IRQ_ATA1);                             // irq#14
    disable_mask_ocw1(IRQ_ATA2);                            // irq#15
}

/**
 * @brief pit initialization
 */
static void
init_pit(void) {
    set_command(SC_CHANNEL0, LOWHIGHBYTE, M3, BINARY);
    set_counter(100);                                       // 100 singal per second
}

/**
 * @brief driver initialization
 */
void
kinit_dirver(void) {
    init_pic();
    init_pit();
}
