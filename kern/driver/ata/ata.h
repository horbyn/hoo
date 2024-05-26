/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_H__
#define __KERN_DRIVER_ATA_ATA_H__

#include "ata_cmd.h"
#include "ata_identify_data.h"
#include "kern/x86.h"
#include "kern/sched/tasks.h"

/**
 * @brief ata device definition
*/
typedef struct ata_device {
    bool                   valid_;
    enum_ata_type_bus      bus_;
    enum_ata_type_bus_wire bus_wire_;
    enum_ata_type_device   device_type_;
    uint16_t               io_port_;
    uint16_t               ctrl_port_;
    uint32_t               device_no_;
    uint32_t               total_sectors_;
    ata_serial_number_t    dev_serial_;
    ata_model_number_t     dev_model_;
} ata_device_t;

/**
 * @brief ata device space
 */
typedef struct ata_space {
    uint32_t     device_amount_;
    int          current_select_;
    ata_device_t *device_info_;
} ata_space_t;

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
    ata_cmd_t cmd_;
    // which thread coressponding this disk req?
    uint32_t thread_;
} atabuff_t;

void atabuff_set(atabuff_t *ibuff, void *buff, uint32_t len, uint32_t lba,
    ata_cmd_t cmd);
ata_space_t *get_ataspace(void);
ata_device_t *get_atadevice(void);
void ata_disable_irqs(void);
void ata_set_cmd(uint32_t dev, uint32_t lba, uint8_t cr, ata_cmd_t cmd, bool is_irq);
void ata_space_init(void);

#endif
