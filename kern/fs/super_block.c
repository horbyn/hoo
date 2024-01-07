/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "super_block.h"

/**
 * @brief Set up the super block
 * 
 * @param sb super block
 * @param free_base free layout base
 * @param level iblock access level
 * @retval true: a new disk
 * @retval false: an existed disk
 */
bool
setup_super_block(super_block_t *sb, uint32_t free_base, enum_index_level_t level) {

    bool is_new = false;

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
        sb->index_level_     = level;

        memmove(sect, sb, sizeof(super_block_t));
        atabuff_set(&ata_buff, sect, sizeof(sect),
            FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_WRITE);
        ata_driver_rw(&ata_buff);

        is_new = true;
    }

    return is_new;
}
