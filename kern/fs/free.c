/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "free.h"

static uint8_t *__bmbuff_fs_free;
static uint32_t __bmbuff_fs_free_inbytes;
static bitmap_t __bmfs_free;

/**
 * @brief get a free block (only for index, not involves disk rw)
 * 
 * @return lba of a free block
 */
lba_index_t
free_allocate() {
    uint32_t bit = bitmap_scan_empty(&__bmfs_free);
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

    bitmap_clear(&__bmfs_free, bit);
}

/**
 * @brief one free block read from / write to disk
 * 
 * @param buff     buffer
 * @param base_lba lba
 * @param cmd      ata command
 * @param is_new   whether the free block is new
 */
void
free_rw_disk(void *buff, lba_index_t base_lba, ata_cmd_t cmd, bool is_new) {
    if (buff == null)    panic("free_rw_disk(): null pointer");
    if (base_lba < __super_block.lba_free_)    panic("free_rw_disk(): invalid lba");
    idx_t bit = base_lba - __super_block.lba_free_;
    if (is_new == false) {
        if (bitmap_test(&__bmfs_free, bit) == false)
            panic("free_rw_disk(): not allow to read from / write to an empty block");
    }

    ata_driver_rw(buff, BYTES_SECTOR, base_lba, cmd);

    if (cmd == ATA_CMD_IO_WRITE)    bitmap_set(&__bmfs_free, bit);
    else    bitmap_clear(&__bmfs_free, bit);
    ata_driver_rw(__bmbuff_fs_free, __bmbuff_fs_free_inbytes,
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

    __bmbuff_fs_free_inbytes = __super_block.map_free_sectors_ * BYTES_SECTOR;
    __bmbuff_fs_free = dyn_alloc(__bmbuff_fs_free_inbytes); // no need to free
    bzero(__bmbuff_fs_free, __bmbuff_fs_free_inbytes);
    bitmap_init(&__bmfs_free,
        __super_block.map_free_sectors_ * BYTES_SECTOR * BITS_PER_BYTE,
        __bmbuff_fs_free);

    ata_driver_rw(__bmbuff_fs_free, __bmbuff_fs_free_inbytes,
        FS_LAYOUT_BASE_MAP_FREE, cmd);
}
