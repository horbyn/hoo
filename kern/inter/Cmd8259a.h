/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_INTER_CMD8259A_H__
#define __KERN_INTER_CMD8259A_H__

#define MAS_EVEN    0x20
#define SLA_EVEN    0xa0

/**
 * @brief x86 icw1 command
 */
typedef enum icw1 {
    ICW1_ICW4 = 1,                                          // ICW4 will be used
    ICW1_SINGLE = 2,                                        // Single (cascade) mode
    ICW1_LEVEL = 8                                          // Level triggered (edge) mode
} icw1_t;

/**
 * @brief x86 icw4 command
 */
typedef enum icw4 {
    ICW4_AUTO = 2,                                          // Auto (normal) EOI
    ICW4_BUF_SLAVE = 8,                                     // Buffered mode/slave
    ICW4_BUF_MASTER = 0xc,                                  // Buffered mode/master
    ICW4_SFNM = 0x10                                        // Special fully nested (not)
} icw4_t;

/***********************************************************
 * I will use only AUTO EOI, so i do not define OCW2&OCW3  *
 ***********************************************************/

/**
 * @brief x86 ocw1 command for disabling master irq
 */
typedef enum irq {
    IRQ_TIMER = 0,                                          // system timer
    IRQ_KEYBOARD,                                           // keyboard on PS/2 port
    IRQ_CASCADE,                                            // cascade(slave accept interrupt from master)
    IRQ_COM2,                                               // serial port controller for serial port2
    IRQ_COM1,                                               // serial port controller for serial port1
    IRQ_LPT2,                                               // line print terminal 2
    IRQ_FLOPPY,                                             // floppy disk controller
    IRQ_LPT1,                                               // line print terminal 1

    IRQ_CMOS_RTC,                                           // cmos real-time clock
    IRQ_ACPI,                                               //  Advanced Configuration and Power Interface
    IRQ_OPEN_SCSI_NIC1,                                     // open interrupt/available, SCSI or NIC
    IRQ_OPEN_SCSI_NIC2,                                     // open interrupt/available, SCSI or NIC
    IRQ_MOUSE,                                              // mouse on PS/2 port
    IRQ_MATH,                                               // match co-processor
    IRQ_ATA1,                                               // primary ATA channel
    IRQ_ATA2                                                // secondary ATA channel
} irq_t;

#endif
