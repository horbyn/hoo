/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_STUFF_H__
#define __DEVICE_ATA_ATA_STUFF_H__

#include "kern/types.h"

#define ATA_MAX_SUPPORTED_DEVICES 4                         // two bus with two devices each
#define BYTES_SECTOR              512

/**
 * @brief ata command register bit
 */
typedef size_t ata_cmd_t;

/**
 * @brief ata bus type
 */
typedef enum ata_type_bus {
    ATA_TYPE_BUS_PRIMARY = 0,
    ATA_TYPE_BUS_SECONDARY,

    ATA_TYPE_BUS_MAX
} enum_ata_type_bus;
#define ENUM2STR_ATA_TYPE_BUS(e) \
    ((e) == ATA_TYPE_BUS_PRIMARY   ? "Primary" :    \
     (e) == ATA_TYPE_BUS_SECONDARY ? "Secondary" :  \
     "Unknown")

/**
 * @brief ata bus wire type
 * 
 */
typedef enum ata_type_bus_wire {
    ATA_TYPE_BUS_WIRE_LOW = 0,                              // master
    ATA_TYPE_BUS_WIRE_HIGH,                                 // slave

    ATA_TYPE_BUS_WIRE_MAX
} enum_ata_type_bus_wire;
#define ENUM2STR_ATA_TYPE_BUS_WIRE(e) \
    ((e) == ATA_TYPE_BUS_WIRE_LOW  ? "Master" :     \
     (e) == ATA_TYPE_BUS_WIRE_HIGH ? "Slave" :      \
     "Unknown")

/**
 * @brief ata device type definition
 */
typedef enum ata_type_device {
    ATA_TYPE_DEVICE_UNKNOWN = 0,
    ATA_TYPE_DEVICE_ATA
} enum_ata_type_device;
#define ENUM2STR_ATA_TYPE_DEVICE(e) \
    ((e) == ATA_TYPE_DEVICE_UNKNOWN ? "Unknown" :   \
     "ATA")

#endif
