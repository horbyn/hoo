/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "disk_rw_test.h"

static char buff[BYTES_SECTOR];
static uint8_t stack[PGSIZE] = { 0 };

/**
 * @brief the actual reading
 */
void
disk_reading() {
#ifdef DEBUG
    kprintf("disk_reading()\n");
#endif

    ata_driver_init();
    bzero(buff, sizeof(buff));

    // read lba. 0 (mbr) from disk to buff
    atabuff_t ata_buff;
    bzero(&ata_buff, sizeof(atabuff_t));
    atabuff_set(&ata_buff, buff, sizeof(buff), 0, ATA_CMD_IO_READ);
    ata_rw(&ata_buff);

    for (size_t i = 0; i < sizeof(buff); ++i)
        kprintf("%x ", buff[i]);
}

/**
 * @brief test the reading from disk
 */
void
test_disk_read() {
#ifdef DEBUG
    kprintf("test_disk_read()\n");
#endif
    // assign to the other thread
    create_kthread(stack, null, disk_reading);

    // main thread spins here
    while(1);
}
