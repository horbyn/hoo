/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "disk_rw_test.h"

static char buff[BYTES_SECTOR];

/**
 * @brief test the reading from disk
 */
void
test_disk_read() {
    ide_init();

    bzero(buff, sizeof(buff));

    // read lba. 0 (mbr) from disk to buff
    idebuff_t ibuff;
    bzero(&ibuff, sizeof(idebuff_t));
    idebuff_set(&ibuff, buff, sizeof(buff), 0, READ);
    ide_rw(&ibuff);
}
