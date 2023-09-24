/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_IDE_H__
#define __DEVICE_IDE_H__

#include "kern/types.h"
#include "kern/x86.h"
#include "kern/lib/queue.h"
#include "kern/lib/lib.h"

#define BYTES_SECTOR                512
#define IDE_DIFF                    0x80
#define IDE_CMDMAP_DATA             0x1f0
#define IDE_CMDMAP_ERROR_FEATURES   0x1f1
#define IDE_CMDMAP_SECTORCR         0x1f2
#define IDE_CMDMAP_LBALOW           0x1f3
#define IDE_CMDMAP_LBAMID           0x1f4
#define IDE_CMDMAP_LBAHIGH          0x1f5
#define IDE_CMDMAP_DEVICE           0x1f6
#define IDE_CMDMAP_STATUS_COMMAND   0x1f7

extern queue_t __queue_ide;

/**
 * @brief ide buffer
 */
typedef struct ide_buff {
    void *buff_;
    size_t len_;
} idebuff_t;

/**
 * @brief difinition of ide register: device
 */
typedef enum ide_register_device {
    MOD_LBA = 0x40,
    DEV_SLA = 0x10
} ider_dev_t;

/**
 * @brief definition of ide command
 */
typedef enum ide_register_command {
    IDENTIFY = 0xec,
    READ = 0x20,
    WRITE = 0x30
} ider_cmd_t;

/**
 * @brief definition of ide status
 */
typedef enum ide_register_status {
    BSY = 0x80,
    READY = 0x40,
    REQ_DONE = 0x8,
    ERR = 0x1
} ider_stat_t;

void ide_init();
void ide_rw(uint32_t, idebuff_t *);

#endif
