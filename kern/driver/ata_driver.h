/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_DRIVER_ATA_DRIVER_H__
#define __KERN_DRIVER_ATA_DRIVER_H__

#include "device/ata.h"
#include "kern/debug.h"
#include "kern/types.h"
#include "kern/x86.h"
#include "kern/lib/lib.h"
#include "kern/lib/queue.h"
#include "kern/lib/spinlock.h"
#include "kern/sched/tasks.h"

#define BYTES_SECTOR                512

extern queue_t __queue_ata;                                 // representation of FIFO disk req
extern queue_t __queue_ata_sleep;                           // disk requests slept here

typedef size_t ata_cmd_t;

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

void ata_driver_init();
void ata_rw(atabuff_t *);
void ata_intr();
void atabuff_set(atabuff_t *, void *, size_t, size_t, ata_cmd_t);

#endif
