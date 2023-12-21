/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "disk_rw_test.h"

static uint8_t buff[BYTES_SECTOR];
static uint8_t stack[PGSIZE] = { 0 };

/**
 * @brief the actual reading
 */
void
disk_reading() {

    //ata_driver_init(ATA_METHOD_IRQ); // not completely implement; don't use
    ata_driver_init(ATA_METHOD_POLLING);
    bzero(buff, sizeof(buff));

    // read lba. 0 (mbr) from disk to buff
    atabuff_t ata_buff;
    bzero(&ata_buff, sizeof(atabuff_t));
    atabuff_set(&ata_buff, buff, sizeof(buff), 0, ATA_CMD_IO_READ);
    ata_driver_rw(&ata_buff);

    for (size_t i = 0; i < sizeof(buff); ++i)
        kprintf("%x ", buff[i]);
}

/**
 * @brief test the reading from disk
 */
void
test_disk_read() {

    // assign to the other thread
    create_kthread(stack, null, disk_reading);

    // main thread spins here
    while(1);
}
