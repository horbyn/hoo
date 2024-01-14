/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "blocks.h"

static uint8_t __fs_map_blocks[BYTES_SECTOR * 259];           // about 130 kb, temporary

/**
 * @brief get a free block (only for index, not involves disk rw)
 * 
 * @return lba of a free block
 */
lba_index_t
block_allocate() {
    uint32_t len = BYTES_SECTOR *
        (__super_block.lba_blocks_ - __super_block.lba_map_blocks_);

    uint32_t bit = bitmap_scan(__fs_map_blocks, len);
    return (__super_block.lba_blocks_ + bit);
}

/**
 * @brief release a block (only for index, not involves disk rw)
 * 
 * @param index lba of the block to be released
 */
void
block_release(lba_index_t index) {
    idx_t bit = index - __super_block.lba_blocks_;
    if (bit == INVALID_INDEX)
        panic("block_release(): invalid lba");

    bitmap_clear(__fs_map_blocks, bit);
}

void
blocks_rw_disk(void *buff, size_t bufflen, lba_index_t base_lba, ata_cmd_t cmd) {
    idx_t bit = base_lba - __super_block.lba_blocks_;
    if (bitmap_test(__fs_map_blocks, bit) == false)
        panic("blocks_rw_disk(): not allow to read from an empty block");

    uint8_t sect[BYTES_SECTOR], *p = (uint8_t *)buff;
    size_t amount = (bufflen / BYTES_SECTOR);
    size_t last_one = bufflen % BYTES_SECTOR;

    for (size_t i = 0; i <= amount; ++i, p += BYTES_SECTOR) {
        size_t cur = (i == amount) ? last_one : BYTES_SECTOR;

        if (cmd == ATA_CMD_IO_WRITE) {
            bzero(sect, BYTES_SECTOR);
            memmove(sect, p, cur);
        }

        ata_driver_rw(sect, BYTES_SECTOR, (base_lba + i), cmd);

        if (cmd == ATA_CMD_IO_READ)
            memmove(p, sect, cur);

        if (cmd == ATA_CMD_IO_WRITE)
            bitmap_set(__fs_map_blocks, bit + i);
        else    bitmap_clear(__fs_map_blocks, bit + i);
        ata_driver_rw(__fs_map_blocks, sizeof(__fs_map_blocks),
            __super_block.lba_map_blocks_, cmd);
    } // end for()

}

/**
 * @brief Set up the free blocks map
 *
 * @param is_new a new disk
 */
void
setup_blocks_map(bool is_new) {
    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_blocks, sizeof(__fs_map_blocks));

    ata_driver_rw(__fs_map_blocks, sizeof(__fs_map_blocks),
        FS_LAYOUT_BASE_MAP_FREE, cmd);
}
