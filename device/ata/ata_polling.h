/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_POLLING_H__
#define __DEVICE_ATA_ATA_POLLING_H__

#include "ata.h"

void ata_polling_init(void);
void ata_polling_rw(atabuff_t *, bool);

#endif
