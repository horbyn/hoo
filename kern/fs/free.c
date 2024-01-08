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
allocate_free_block() {
    uint32_t bit = 0;
    uint32_t bitmap_blocks =
        __super_block.lba_free_ - __super_block.lba_map_free_;

    for (; bit < bitmap_blocks * BYTES_SECTOR; ++bit) {
        if (bitmap_test(__fs_map_free, bit) == false) {
            bitmap_set(__fs_map_free, bit);
            break;
        }
    }

    if (bit == bitmap_blocks * BYTES_SECTOR)
        panic("allocate_free_block(): no more free blocks");
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

    atabuff_t ata_buff;
    atabuff_set(&ata_buff, __fs_map_free, sizeof(__fs_map_free),
        FS_LAYOUT_BASE_MAP_FREE, cmd);
    ata_driver_rw(&ata_buff);
}
