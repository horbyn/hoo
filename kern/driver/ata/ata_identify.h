/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_IDENTIFY_H__
#define __KERN_DRIVER_ATA_ATA_IDENTIFY_H__

#include "ata_stuff.h"
#include "kern/types.h"
#include "kern/driver/io.h"
#include "kern/lib/lib.h"

#define SIZE_ATA_STRING_BUFF 1024

/**
 * @brief ata serial number
 */
typedef struct ata_serial_number {
    uint16_t serial_number_[10];
} ataser_t;

/**
 * @brief ata model number
 */
typedef struct ata_model_number {
    uint16_t model_number_[20];
} atamod_t;

/**
 * @brief ata identify data
 */
typedef struct ata_identify_data {
    uint16_t :15;
    // general configuration
    uint16_t word0_ :1;

    uint16_t word1_9_[9];
    // device serial number
    ataser_t word10_19_;

    uint16_t word20_26_[7];
    // device model number
    atamod_t word27_46_;

    uint16_t word47_59_[13];
    // total sectors
    uint16_t word60_61_[2];

    uint16_t word62_255_[194];
} ataid_t;

ata_t    ata_get_device_type(const ataid_t *);
uint32_t ata_get_sectors(const ataid_t *);
void     ata_get_serial_number(const ataid_t *, ataser_t *);
void     ata_get_model_number(const ataid_t *, atamod_t *);

#endif
