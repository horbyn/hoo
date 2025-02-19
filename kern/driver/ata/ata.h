#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_H__
#define __KERN_DRIVER_ATA_ATA_H__

#include "ata_cmd.h"

/**
 * @brief ATA 方式（POLLING 或 IRQ）
 */
typedef uint32_t atamth_t;

#define ATA_METHOD_POLLING  0
#define ATA_METHOD_IRQ      1

void init_ata(void);
void ata_driver_change_mode(atamth_t method);
void ata_driver_rw(void *buff, uint32_t bufflen, int lba, atacmd_t cmd);

#endif
