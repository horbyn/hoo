/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "inodes.h"

/**
 * @brief Set up inode metadata
 */
void
setup_inode(bool is_new) {

    // a new disk
    if (is_new) {
        // writes to inode map layout
        uint8_t sect[BYTES_SECTOR];
        bzero(sect, sizeof(sect));
        atabuff_t ata_buff;
        atabuff_set(&ata_buff, sect, sizeof(sect),
            FS_LAYOUT_BASE_MAP_INODES, ATA_CMD_IO_WRITE);
        ata_driver_rw(&ata_buff);

        // writes to inodes layout
        for (size_t i = 0; i < MAX_INODES; ++i) {
            atabuff_set(&ata_buff, sect, sizeof(sect),
                FS_LAYOUT_BASE_INODES + i, ATA_CMD_IO_WRITE);
            ata_driver_rw(&ata_buff);
        }
    }
}
