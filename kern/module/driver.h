/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_MODULE_DRIVER_H__
#define __KERN_MODULE_DRIVER_H__

#include "kern/driver/ata/ata_polling.h"
#include "kern/driver/ata/ata_irq.h"
#include "kern/driver/8259a/8259a.h"
#include "kern/driver/8253/8253.h"

/**
 * @brief ata method like IRQs or polling
 */
typedef enum ata_method {
    ATA_METHOD_POLLING = 0,
    ATA_METHOD_IRQ
} enum_ata_method;

void kinit_dirver(void);
void ata_driver_init(enum_ata_method);
void ata_driver_rw(void *buff, uint32_t bufflen, idx_t lba, ata_cmd_t cmd);

#endif
