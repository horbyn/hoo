#pragma once
#ifndef __KERN_DRIVER_8259A_8259A_STUFF_H__
#define __KERN_DRIVER_8259A_8259A_STUFF_H__

#define MAS_EVEN            0x20
#define SLA_EVEN            0xa0

// 需要使用 ICW4
#define ICW1_ICW4           1
// 单级（级联）模式
#define ICW1_SINGLE         2
// 水平触发（边缘触发）模式
#define ICW1_LEVEL          8

// 自动 EOI
#define ICW4_AUTO           2
// Buffered mode/slave
#define ICW4_BUF_SLAVE      8
// Buffered mode/master
#define ICW4_BUF_MASTER     0xc
// Special fully nested
#define ICW4_SFNM           0x10

/***********************************************************
 *       由于只使用自动 EOI，所以就不定义 OCW2 和 OCW3 了       *
 ***********************************************************/

// 系统计时器
#define IRQ_TIMER           0
// PS/2 接口键盘
#define IRQ_KEYBOARD        1
// 级联（从片可以接收从主片那里转发的中断）
#define IRQ_CASCADE         2
// COM2 串口控制器
#define IRQ_COM2            3
// COM1 串口控制器
#define IRQ_COM1            4
// 行打印设备 2
#define IRQ_LPT2            5
// 软盘控制器
#define IRQ_FLOPPY          6
// 行打印设备 1
#define IRQ_LPT1            7
// cmos 实时时钟
#define IRQ_CMOS_RTC        8
// 高级配置和电源接口
#define IRQ_ACPI            9
// open interrupt/available, SCSI or NIC
#define IRQ_OPEN_SCSI_NIC1  0xa
// open interrupt/available, SCSI or NIC
#define IRQ_OPEN_SCSI_NIC2  0xb
// PS/2 接口鼠标
#define IRQ_MOUSE           0xc
// 数学协处理器
#define IRQ_MATH            0xd
// primary ATA 通道
#define IRQ_ATA1            0xe
// secondary ATA 通道
#define IRQ_ATA2            0xf

#endif
