#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_IDENTIFY_H__
#define __KERN_DRIVER_ATA_ATA_IDENTIFY_H__

#include "ata_stuff.h"

#define SIZE_ATA_STRING_BUFF 1024

/**
 * @brief ATA 设备序列号
 */
typedef struct ata_serial_number {
    uint16_t serial_number_[10];
} ataser_t;

/**
 * @brief ATA 设备 model number
 */
typedef struct ata_model_number {
    uint16_t model_number_[20];
} atamod_t;

/**
 * @brief ATA IDENTIFY 数据
 */
typedef struct ata_identify_data {
    uint16_t :15;
    // 通用配置
    uint16_t word0_ :1;

    uint16_t word1_9_[9];
    // 设备序列号
    ataser_t word10_19_;

    uint16_t word20_26_[7];
    // 设备 model number
    atamod_t word27_46_;

    uint16_t word47_59_[13];
    // 总扇区数
    uint16_t word60_61_[2];

    uint16_t word62_255_[194];
} ataid_t;

ata_t    ata_get_device_type(const ataid_t *);
uint32_t ata_get_sectors(const ataid_t *);
void     ata_get_serial_number(const ataid_t *, ataser_t *);
void     ata_get_model_number(const ataid_t *, atamod_t *);

#endif
