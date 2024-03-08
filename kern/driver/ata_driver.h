/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_DRIVER_H__
#define __KERN_DRIVER_ATA_DRIVER_H__

#include "device/ata/ata_irq.h"
#include "device/ata/ata_polling.h"
#include "kern/debug.h"
#include "kern/types.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"

/**
 * @brief ata method likes IRQs or polling
 */
typedef enum ata_method {
    ATA_METHOD_POLLING = 0,
    ATA_METHOD_IRQ
} enum_ata_method;

void ata_driver_init(enum_ata_method);
void ata_driver_rw(void *buff, size_t bufflen, idx_t lba, ata_cmd_t cmd);

#endif
