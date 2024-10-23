/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_H__
#define __KERN_DRIVER_ATA_ATA_H__

#include "ata_polling.h"

/**
 * @brief ata method like IRQs or polling
 */
typedef uint32_t atamth_t;

#define ATA_METHOD_POLLING  0
#define ATA_METHOD_IRQ      1

void init_ata(void);
void ata_driver_change_mode(atamth_t method);
void ata_driver_rw(void *buff, uint32_t bufflen, int lba, atacmd_t cmd);

#endif
