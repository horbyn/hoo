/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "fs.h"

/**
 * @brief Set up the free blocks map
 */
static void
setup_free_map() {
    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    atabuff_t ata_buff;
    atabuff_set(&ata_buff, sect, sizeof(sect),
        FS_LAYOUT_BASE_MAP_FREE, ATA_CMD_IO_WRITE);
    ata_driver_rw(&ata_buff);
}

/**
 * @brief initialize the disk layout
 */
void
init_fs(void) {
    /****************************************************************************
     * DISK LAYOUT :                                                            *
     * ┌──────────┬─────────────┬────────────┬────────────┬──────────┬──────┐   *
     * │ RESERVED │ SUPER_BLOCK │ MAP_INODES │   INODES   │ MAP_FREE │ FREE │   *
     * └──────────┴─────────────┴────────────┴────────────┴──────────┴──────┘   *
     * |          \             \            \            \          \          *
     * | 1 sector  \  1 sector   \  1 sector  \ 64 sectors \ 1 sector \         *
     * |   LBA-0    \    LBA-1    \   LBA-2    \ LBA-3..66  \ LBA-67.. \ LBA-.. *
     *                                                                          *
     * NOTE : the first sector will, if necessary, use for Partition            *
     ****************************************************************************/

    uint32_t dev_sectors =
        ata_space.device_info_[ata_space.current_select_].total_sectors_;

    super_block_t super_block;
    bzero(&super_block, sizeof(super_block_t));
    uint32_t free_lba = get_necessary_sectors(&super_block, dev_sectors);
    if (free_lba == 0)    panic("init_fs()");
    setup_super_block(&super_block, free_lba);

    if (super_block.magic_ != FS_HOO_MAGIC) {
        // a new disk
        setup_inode(true);
        setup_free_map();
        setup_root_dir();

        super_block.magic_ = FS_HOO_MAGIC;
    }
}
