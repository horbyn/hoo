#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_POLLING_H__
#define __KERN_DRIVER_ATA_ATA_POLLING_H__

#include "ata_device.h"

void ata_polling_init(void);
void ata_polling_rw(atabuff_t *buff);

#endif