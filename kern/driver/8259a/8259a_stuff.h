/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_8259A_8259A_STUFF_H__
#define __KERN_DRIVER_8259A_8259A_STUFF_H__

#define MAS_EVEN            0x20
#define SLA_EVEN            0xa0

// ICW4 will be used
#define ICW1_ICW4           1
// Single (cascade) mode
#define ICW1_SINGLE         2
// Level triggered (edge) mode
#define ICW1_LEVEL          8

// Auto (normal) EOI
#define ICW4_AUTO           2
// Buffered mode/slave
#define ICW4_BUF_SLAVE      8
// Buffered mode/master
#define ICW4_BUF_MASTER     0xc
// Special fully nested (not)
#define ICW4_SFNM           0x10

/***********************************************************
 * I will use only AUTO EOI, so i do not define OCW2&OCW3  *
 ***********************************************************/

// system timer
#define IRQ_TIMER           0
// keyboard on PS/2 port
#define IRQ_KEYBOARD        1
// cascade(slave accept interrupt from master)
#define IRQ_CASCADE         2
// serial port controller for serial port2
#define IRQ_COM2            3
// serial port controller for serial port1
#define IRQ_COM1            4
// line print terminal 2
#define IRQ_LPT2            5
// floppy disk controller
#define IRQ_FLOPPY          6
// line print terminal 1
#define IRQ_LPT1            7
// cmos real-time clock
#define IRQ_CMOS_RTC        8
//  Advanced Configuration and Power Interface
#define IRQ_ACPI            9
// open interrupt/available, SCSI or NIC
#define IRQ_OPEN_SCSI_NIC1  0xa
// open interrupt/available, SCSI or NIC
#define IRQ_OPEN_SCSI_NIC2  0xb
// mouse on PS/2 port
#define IRQ_MOUSE           0xc
// match co-processor
#define IRQ_MATH            0xd
// primary ATA channel
#define IRQ_ATA1            0xe
// secondary ATA channel
#define IRQ_ATA2            0xf

#endif
