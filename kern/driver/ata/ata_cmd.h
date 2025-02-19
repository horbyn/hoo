#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_CMD_H__
#define __KERN_DRIVER_ATA_ATA_CMD_H__

#include "kern/x86.h"

// ATA 端口起始地址
#define ATA_PRIMARY_PORT_IO_BASE        0x1f0
#define ATA_SECONDARY_PORT_IO_BASE      0x170
#define ATA_PRIMARY_PORT_CTRL_BASE      0x3f6
#define ATA_SECONDARY_PORT_CTRL_BASE    0x376

// ATA IO 端口偏移
#define ATA_IO_RW_OFFSET_DATA           0x00
#define ATA_IO_R_OFFSET_ERROR           0x01
#define ATA_IO_W_OFFSET_FEATURE         ATA_IO_R_OFFSET_ERROR
#define ATA_IO_RW_OFFSET_SECTOR_COUNT   0x02
#define ATA_IO_RW_OFFSET_LBA_LOW        0x03
#define ATA_IO_RW_OFFSET_LBA_MID        0x04
#define ATA_IO_RW_OFFSET_LBA_HIGH       0x05
#define ATA_IO_RW_OFFSET_DRIVE_SELECT   0x06
#define ATA_IO_W_OFFSET_COMMAND         0x07
#define ATA_IO_R_OFFSET_STATUS          ATA_IO_W_OFFSET_COMMAND

// ATA 控制端口偏移
#define ATA_CTRL_R_OFFSET_ALT_STATUS    0x00
#define ATA_CTRL_W_OFFSET_DEV_CONTROL   0x00
#define ATA_CTRL_RW_OFFSET_DRIVE_ADDR   0x01

// error IO 寄存器各字段
#define ATA_ERROR_AMNF                  0x01
#define ATA_ERROR_TK0NF                 0x02
#define ATA_ERROR_ABRT                  0x04
#define ATA_ERROR_MCR                   0x08
#define ATA_ERROR_IDNF                  0x10
#define ATA_ERROR_MC                    0x20
#define ATA_ERROR_UNC                   0x40
#define ATA_ERROR_BBK                   0x80

// 状态 IO 寄存器各字段
#define ATA_STATUS_ERR                  0x01
#define ATA_STATUS_IDX                  0x02
#define ATA_STATUS_CORR                 0x04
#define ATA_STATUS_DRQ                  0x08
#define ATA_STATUS_SRV                  0x10
#define ATA_STATUS_DF                   0x20
#define ATA_STATUS_RDY                  0x40
#define ATA_STATUS_BSY                  0x80

// 命令字
#define ATA_CMD_IO_IDENTIFY             0xec
#define ATA_CMD_IO_READ                 0x20
#define ATA_CMD_IO_WRITE                0x30

// device IO 寄存器各字段
#define ATA_DEV_IO_DEV_SLAVE            0x10
#define ATA_DEV_IO_MOD_LBA              0x40

// device 控制寄存器各字段
#define ATA_DEV_CTRL_NIEN               0x02
#define ATA_DEV_CTRL_SRST               0x04
#define ATA_DEV_CTRL_HOB                0x80

/**
 * @brief ATA 命令字
 */
typedef uint32_t atacmd_t;

#endif
