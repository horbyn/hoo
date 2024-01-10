/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "free.h"

static uint8_t __fs_map_free[BYTES_SECTOR * 259];           // about 130 kb, temporary

/**
 * @brief get a free block
 * 
 * @return lba where the free block is
 */
uint32_t
free_block_allocate() {
    uint32_t len = BYTES_SECTOR *
        (__super_block.lba_free_ - __super_block.lba_map_free_);

    uint32_t bit = bitmap_scan(__fs_map_free, len);
    return (__super_block.lba_free_ + bit);
}

/**
 * @brief Set up the free blocks map
 *
 * @param is_new   a new disk
 */
void
setup_free_map(bool is_new) {
    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_free, sizeof(__fs_map_free));

    ata_driver_rw(__fs_map_free, sizeof(__fs_map_free),
        FS_LAYOUT_BASE_MAP_FREE, cmd);
}
