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
    // level; cascade; icw4
    set_icw1(ICW1_ICW4);
    set_icw2(0x20, 0x28);
    set_icw3(2);
    // CFNM; no-buffer; normal EOI
    set_icw4(ICW4_AUTO);

    // irq#0
    enable_mask_ocw1(IRQ_TIMER);
    // irq#1
    disable_mask_ocw1(IRQ_KEYBOARD);
    // irq#2
    disable_mask_ocw1(IRQ_CASCADE);
    // irq#3
    disable_mask_ocw1(IRQ_COM2);
    // irq#4
    disable_mask_ocw1(IRQ_COM1);
    // irq#5
    disable_mask_ocw1(IRQ_LPT2);
    // irq#6
    disable_mask_ocw1(IRQ_FLOPPY);
    // irq#7
    disable_mask_ocw1(IRQ_LPT1);
    // irq#8
    disable_mask_ocw1(IRQ_CMOS_RTC);
    // irq#9
    disable_mask_ocw1(IRQ_ACPI);
    // irq#10
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC1);
    // irq#11
    disable_mask_ocw1(IRQ_OPEN_SCSI_NIC2);
    // irq#12
    disable_mask_ocw1(IRQ_MOUSE);
    // irq#13
    disable_mask_ocw1(IRQ_MATH);
    // irq#14
    disable_mask_ocw1(IRQ_ATA1);
    // irq#15
    disable_mask_ocw1(IRQ_ATA2);
}

/**
 * @brief pit initialization
 */
static void
init_pit(void) {
    set_command(SC_CHANNEL0, LOWHIGHBYTE, M3, BINARY);
    // n singals per second
    set_counter(TICKS_PER_SEC);
}

/**
 * @brief driver initialization
 */
void
kinit_driver(void) {
    init_pic();
    init_pit();
    init_ata();
}
