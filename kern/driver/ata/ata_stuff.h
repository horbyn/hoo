/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_STUFF_H__
#define __KERN_DRIVER_ATA_ATA_STUFF_H__

#include "kern/x86.h"

#define BYTES_SECTOR    512

/* ata bus type */

#define ATA_TYPE_BUS_PRIMARY        0
#define ATA_TYPE_BUS_SECONDARY      1
#define ATA_TYPE_BUS_MAX            2
#define MACRO2STR_ATA_TYPE_BUS(m) \
    ((m) == ATA_TYPE_BUS_PRIMARY   ? "Primary" :    \
     (m) == ATA_TYPE_BUS_SECONDARY ? "Secondary" :  \
     "Unknown")

/* ata bus wire type */

#define ATA_TYPE_BUS_WIRE_LOW       0
#define ATA_TYPE_BUS_WIRE_HIGH      1
#define ATA_TYPE_BUS_WIRE_MAX       2
#define MACRO2STR_ATA_TYPE_BUS_WIRE(m) \
    ((m) == ATA_TYPE_BUS_WIRE_LOW  ? "Master" :     \
     (m) == ATA_TYPE_BUS_WIRE_HIGH ? "Slave" :      \
     "Unknown")

/* ata device type definition */

#define ATA_TYPE_DEVICE_UNKNOWN     0
#define ATA_TYPE_DEVICE_ATA         1
#define MACRO2STR_ATA_TYPE_DEVICE(m) \
    ((m) == ATA_TYPE_DEVICE_UNKNOWN ? "Unknown" :   \
     "ATA")

/**
 * @brief ata stuff
 */
typedef uint32_t ata_t;

// two bus with two devices each
#define ATA_MAX_SUPPORTED_DEVICES   (ATA_TYPE_BUS_MAX * ATA_TYPE_BUS_WIRE_MAX)

#endif
