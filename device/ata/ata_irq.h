/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_IRQ_H__
#define __DEVICE_ATA_ATA_IRQ_H__

#include "ata.h"
#include "kern/debug.h"
#include "kern/lib/queue.h"
#include "kern/lib/spinlock.h"

void ata_irq_init();
void ata_irq_intr();
void ata_irq_rw(atabuff_t *);

#endif
