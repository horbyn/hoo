/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_ATA_IRQ_H__
#define __KERN_DRIVER_ATA_ATA_IRQ_H__

#include "ata.h"
#include "kern/units/queue.h"

void ata_irq_init();
void ata_irq_intr();
void ata_irq_rw(atabuff_t *buff, bool is_irq);

#endif
