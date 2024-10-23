/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_DEVICE_H__
#define __KERN_DRIVER_ATA_ATA_DEVICE_H__

#include "ata_cmd.h"
#include "ata_identify.h"

/**
 * @brief ata device definition
*/
typedef struct ata_device {
    bool     valid_;
    ata_t    bus_;
    ata_t    bus_wire_;
    ata_t    device_type_;
    uint16_t io_port_;
    uint16_t ctrl_port_;
    uint32_t device_no_;
    uint32_t total_sectors_;
    ataser_t dev_serial_;
    atamod_t dev_model_;
} atadev_t;

/**
 * @brief ata device space
 */
typedef struct ata_space {
    uint32_t device_amount_;
    int      current_select_;
    atadev_t *device_info_;
} ataspc_t;

/**
 * @brief ata buffer
 */
typedef struct ata_buff {
    // buffer pointer
    void *buff_;
    // buffer size
    uint32_t len_;
    // lba no.
    uint32_t lba_;
    // operation command
    atacmd_t cmd_;
} atabuff_t;

void     atabuff_set(atabuff_t *ibuff, void *buff, uint32_t len, int lba,
    atacmd_t cmd);
ataspc_t *get_ataspace(void);
atadev_t *get_atadevice(void);
void     ata_disable_irqs(void);
void     ata_set_cmd(uint32_t dev, uint32_t lba, uint8_t cr, atacmd_t cmd,
    bool is_irq);
void     ata_detect(void);

#endif
