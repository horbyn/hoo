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
    ide_init();

    bzero(buff, sizeof(buff));

    // read lba. 0 (mbr) from disk to buff
    idebuff_t ibuff;
    bzero(&ibuff, sizeof(idebuff_t));
    idebuff_set(&ibuff, buff, sizeof(buff), 0, READ);
    ide_rw(&ibuff);

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
