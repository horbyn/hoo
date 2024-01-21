/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "free.h"

static uint8_t __fs_map_free[BYTES_SECTOR * 259];           // about 130 kb, temporary

/**
 * @brief get a free block (only for index, not involves disk rw)
 * 
 * @return lba of a free block
 */
lba_index_t
free_allocate() {
    uint32_t len = BYTES_SECTOR *
        (__super_block.lba_free_ - __super_block.lba_map_free_);

    uint32_t bit = bitmap_scan(__fs_map_free, len);
    return (__super_block.lba_free_ + bit);
}

/**
 * @brief release a block (only for index, not involves disk rw)
 * 
 * @param index lba of the block to be released
 */
void
free_release(lba_index_t index) {
    idx_t bit = index - __super_block.lba_free_;
    if (bit == INVALID_INDEX)
        panic("free_release(): invalid lba");

    bitmap_clear(__fs_map_free, bit);
}

void
free_rw_disk(void *buff, size_t bufflen, lba_index_t base_lba, ata_cmd_t cmd) {
    idx_t bit = base_lba - __super_block.lba_free_;
    if (bitmap_test(__fs_map_free, bit) == false)
        panic("free_rw_disk(): not allow to read from an empty block");

    uint8_t sect[BYTES_SECTOR], *p = (uint8_t *)buff;
    size_t amount = (bufflen / BYTES_SECTOR);
    size_t last_one = bufflen % BYTES_SECTOR;

    for (size_t i = 0; i <= amount; ++i, p += BYTES_SECTOR) {
        size_t cur = (i == amount) ? last_one : BYTES_SECTOR;
        if (cur == 0)    break;

        if (cmd == ATA_CMD_IO_WRITE) {
            bzero(sect, BYTES_SECTOR);
            memmove(sect, p, cur);
        }

        ata_driver_rw(sect, BYTES_SECTOR, (base_lba + i), cmd);

        if (cmd == ATA_CMD_IO_READ)
            memmove(p, sect, cur);

        if (cmd == ATA_CMD_IO_WRITE)
            bitmap_set(__fs_map_free, bit + i);
        else    bitmap_clear(__fs_map_free, bit + i);
    } // end for()

    ata_driver_rw(__fs_map_free, sizeof(__fs_map_free),
        __super_block.lba_map_free_, cmd);

}

/**
 * @brief Set up the free blocks map
 *
 * @param is_new a new disk
 */
void
setup_free_map(bool is_new) {
    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_free, sizeof(__fs_map_free));

    ata_driver_rw(__fs_map_free, sizeof(__fs_map_free),
        FS_LAYOUT_BASE_MAP_FREE, cmd);
}
