/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_IDENTIFY_DATA_LOG_H__
#define __DEVICE_ATA_ATA_IDENTIFY_DATA_LOG_H__

/**
 * @brief ata type definition
 */
typedef enum ata_type {
    PATA = 0,
    SATA
} enum_ata_type;

enum_ata_type ata_stub_get_ata_type(const void *);

#endif
