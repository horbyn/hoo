/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_H__
#define __DEVICE_ATA_ATA_H__

#include "ata_cmd.h"
#include "ata_stuff.h"
#include "ata_identify_data.h"
#include "kern/debug.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/sched/tasks.h"

/**
 * @brief ata device definition
*/
typedef struct ata_device {
    bool                   valid_;
    enum_ata_type_bus      bus_;
    enum_ata_type_bus_wire bus_wire_;
    enum_ata_type_device   device_type_;
    port_t                 io_port_;
    port_t                 ctrl_port_;
    size_t                 device_no_;
    uint32_t               total_sectors_;
    ata_serial_number_t    dev_serial_;
    ata_model_number_t     dev_model_;
} ata_device_t;

/**
 * @brief ata device space
 */
typedef struct ata_space {
    size_t device_amount_;
    int current_select_;
    ata_device_t *device_info_;
} ata_space_t;
extern ata_space_t ata_space;

/**
 * @brief ata buffer
 */
typedef struct ata_buff {
    void *buff_;                                            // buffer pointer
    size_t len_;                                            // buffer size
    size_t lba_;                                            // lba no.
    ata_cmd_t cmd_;                                         // operation command
    size_t thread_;                                         // which thread coressponding this disk req?
} atabuff_t;

void atabuff_set(atabuff_t *, void *, size_t, size_t, ata_cmd_t);
void ata_set_cmd(uint32_t, uint32_t, uint8_t, ata_cmd_t, bool);
void ata_disable_irqs();
void ata_enable_irqs();
void ata_wait_not_busy_but_ready();
void ata_wait_not_busy();
void ata_space_init(void);

#endif
