/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_8259A_CMD_8259A_H__
#define __KERN_DRIVER_8259A_CMD_8259A_H__

#define MAS_EVEN    0x20
#define SLA_EVEN    0xa0

/**
 * @brief x86 icw1 command
 */
typedef enum icw1 {
    // ICW4 will be used
    ICW1_ICW4 = 1,
    // Single (cascade) mode
    ICW1_SINGLE = 2,
    // Level triggered (edge) mode
    ICW1_LEVEL = 8
} icw1_t;

/**
 * @brief x86 icw4 command
 */
typedef enum icw4 {
    ICW4_NONE = 0,
    // Auto (normal) EOI
    ICW4_AUTO = 2,
    // Buffered mode/slave
    ICW4_BUF_SLAVE = 8,
    // Buffered mode/master
    ICW4_BUF_MASTER = 0xc,
    // Special fully nested (not)
    ICW4_SFNM = 0x10
} icw4_t;

/***********************************************************
 * I will use only AUTO EOI, so i do not define OCW2&OCW3  *
 ***********************************************************/

/**
 * @brief x86 ocw1 command for disabling master irq
 */
typedef enum irq {
    // system timer
    IRQ_TIMER = 0,
    // keyboard on PS/2 port
    IRQ_KEYBOARD,
    // cascade(slave accept interrupt from master)
    IRQ_CASCADE,
    // serial port controller for serial port2
    IRQ_COM2,
    // serial port controller for serial port1
    IRQ_COM1,
    // line print terminal 2
    IRQ_LPT2,
    // floppy disk controller
    IRQ_FLOPPY,
    // line print terminal 1
    IRQ_LPT1,

    // cmos real-time clock
    IRQ_CMOS_RTC,
    //  Advanced Configuration and Power Interface
    IRQ_ACPI,
    // open interrupt/available, SCSI or NIC
    IRQ_OPEN_SCSI_NIC1,
    // open interrupt/available, SCSI or NIC
    IRQ_OPEN_SCSI_NIC2,
    // mouse on PS/2 port
    IRQ_MOUSE,
    // match co-processor
    IRQ_MATH,
    // primary ATA channel
    IRQ_ATA1,
    // secondary ATA channel
    IRQ_ATA2
} irq_t;

#endif
