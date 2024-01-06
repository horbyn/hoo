/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "super_block.h"

/**
 * @brief Get the necessary sectors
 * 
 * @param sb the super block
 * @param dev_sec sectors the device had
 * @retval 0: error
 */
uint32_t
get_necessary_sectors(super_block_t *sb, uint32_t dev_sec) {

    const uint32_t MAX_DIRECT = 6,
        ITEMS_PER_SECTOR = BYTES_SECTOR / sizeof(lba_index_t),
        BASE_MAP_FREE = FS_LAYOUT_BASE_MAP_FREE;

    // For direct, all 8 bytes of inode block are direct.
    //   So a file occupies `MAX_INODE_BLOCKS(8)` sectors.
    //   Its bitmap occupies "`FREE_BITMAP_SEC_0`" sectors
    const uint32_t FREE_SEC_0 = MAX_INODES * MAX_INODE_BLOCKS,
        FREE_BITMAP_SEC_0 = (FREE_SEC_0 / 8 / BYTES_SECTOR) + 1,
        SEC_DIRECT = BASE_MAP_FREE + FREE_BITMAP_SEC_0 + FREE_SEC_0;
    // For single indirect, the previous 6 bytes are direct
    //   and the later 2 bytes single indirect. So a file occupies
    //   "`MAX_DIRECT`(6) + `ITEMS_PER_SECTOR`(128) * 2" sectors.
    //   Its bitmap as same as above
    const uint32_t FREE_SEC_1 =
        MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR * 2),
        FREE_BITMAP_SEC_1 = (FREE_SEC_1 / 8 / BYTES_SECTOR) + 1,
        SEC_INDIRECT1 = BASE_MAP_FREE + FREE_BITMAP_SEC_1 + FREE_SEC_1;
    // For double indirect, the previous 6 bytes are direct,
    //   the following one is single indirect, the last one is
    //   double indirect. So a file occupies "`MAX_DIRECT`(6) 
    //   + `ITEMS_PER_SECTOR`(128) + `ITEMS_PER_SECTOR`^2 (128^2)"
    //   sectors. Its bitmap as same as above
    const uint32_t FREE_SEC_2 =
        MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR
        + ITEMS_PER_SECTOR * ITEMS_PER_SECTOR),
        FREE_BITMAP_SEC_2 = (FREE_SEC_2 / 8 / BYTES_SECTOR) + 1,
        SEC_INDIRECT2 = BASE_MAP_FREE + FREE_BITMAP_SEC_2 + FREE_SEC_2;

    if (dev_sec < SEC_DIRECT)
        panic("setup_inode_level()");
    else if (SEC_DIRECT <= dev_sec && dev_sec < SEC_INDIRECT1) {
        sb->index_level_ = INDEX_LEVEL0;
        return BASE_MAP_FREE + FREE_BITMAP_SEC_0;
    } else if (SEC_INDIRECT1 <= dev_sec && dev_sec < SEC_INDIRECT2) {
        sb->index_level_ = INDEX_LEVEL1;
        return BASE_MAP_FREE + FREE_BITMAP_SEC_1;
    } else {
        sb->index_level_ = INDEX_LEVEL2;
        return BASE_MAP_FREE + FREE_BITMAP_SEC_2;
    }

    return 0;
}

/**
 * @brief Set up the super block
 * 
 * @param sb super block
 * @param free_base free layout base
 */
void
setup_super_block(super_block_t *sb, uint32_t free_base) {

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    atabuff_t ata_buff;

    atabuff_set(&ata_buff, sect, sizeof(sect),
        FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_READ);
    ata_driver_rw(&ata_buff);
    memmove(sb, sect, sizeof(super_block_t));

    if (sb->magic_ != FS_HOO_MAGIC) {
        sb->magic_           = FS_HOO_MAGIC;
        sb->lba_partition_   = FS_LAYOUT_BASE_PARTITION;
        sb->lba_super_block_ = FS_LAYOUT_BASE_SUPERBLOCK;
        sb->lba_map_inode_   = FS_LAYOUT_BASE_MAP_INODES;
        sb->lba_inodes_      = FS_LAYOUT_BASE_INODES;
        sb->lba_map_free_    = FS_LAYOUT_BASE_MAP_FREE;
        sb->lba_free_        = free_base;

        memmove(sect, sb, sizeof(super_block_t));
        atabuff_set(&ata_buff, sect, sizeof(sect),
            FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_WRITE);
        ata_driver_rw(&ata_buff);

        sb->magic_ = 0;                                     // a new disk now
    }

}

void
setup_root_dir() {
}
