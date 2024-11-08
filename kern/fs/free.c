/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "free.h"
#include "kern/panic.h"
#include "kern/driver/ata/ata.h"
#include "kern/dyn/dynamic.h"
#include "kern/utilities/bitmap.h"
#include "user/lib.h"

static uint8_t *__bmbuff_fs_free;
static uint32_t __bmbuff_fs_free_inbytes;
static bitmap_t __bmfs_free;

/**
 * @brief Set up the free blocks map
 *
 * @param is_new a new disk
 */
void
setup_free_map(bool is_new) {
    atacmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;

    __bmbuff_fs_free_inbytes = __super_block.map_free_sectors_ * BYTES_SECTOR;
    __bmbuff_fs_free = dyn_alloc(__bmbuff_fs_free_inbytes); // no need to free
    bzero(__bmbuff_fs_free, __bmbuff_fs_free_inbytes);
    bitmap_init(&__bmfs_free,
        __super_block.map_free_sectors_ * BYTES_SECTOR * BITS_PER_BYTE,
        __bmbuff_fs_free);

    ata_driver_rw(__bmbuff_fs_free, __bmbuff_fs_free_inbytes,
        FS_LAYOUT_BASE_MAP_FREE, cmd);
}

/**
 * @brief get a free block
 * 
 * @return lba of a free block
 */
uint32_t
free_allocate() {
    uint32_t bit = bitmap_scan_empty(&__bmfs_free);
    return (__super_block.lba_free_ + bit);
}

/**
 * @brief setup free bitmap (only for index, not involves disk rw)
 * 
 * @param index  free index
 * @param is_set whether to set or clear
 */
void
free_map_setup(uint32_t index, bool is_set) {
    int bit = index - __super_block.lba_free_;
    if (bit == INVALID_INDEX)
        panic("free_release(): invalid lba");

    if (is_set)    bitmap_set(&__bmfs_free, bit);
    else    bitmap_clear(&__bmfs_free, bit);
}

/**
 * @brief update free bitmap to disk
 */
void
free_map_update() {
    ata_driver_rw(__bmbuff_fs_free, __bmbuff_fs_free_inbytes,
        __super_block.lba_map_free_, ATA_CMD_IO_WRITE);
}

/**
 * @brief one free block read from / write to disk
 * 
 * @param buff     buffer
 * @param base_lba lba
 * @param cmd      ata command
 */
void
free_rw_disk(void *buff, uint32_t base_lba, atacmd_t cmd) {
    if (buff == null)    panic("free_rw_disk(): null pointer");
    if (base_lba < __super_block.lba_free_)    panic("free_rw_disk(): invalid lba");
    if (bitmap_test(&__bmfs_free, base_lba - __super_block.lba_free_) == false)
        panic("free_rw_disk(): not allow to read from / write to an empty block");

    ata_driver_rw(buff, BYTES_SECTOR, base_lba, cmd);
}
