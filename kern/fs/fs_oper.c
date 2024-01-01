/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "fs_oper.h"

static enum_indoe_level_t __fs_inode_level;

void
setup_super_block(super_block_t *sb, uint32_t necessary_sectors,
uint32_t dev_sectors) {
    if (dev_sectors < necessary_sectors + 1)
        panic("setup_super_block()");

    sb->lba_partition_ = 0;
    sb->lba_super_block_ = sb->lba_partition_
        + FS_LAYOUT_SECTORS_PARTITION;
    sb->lba_map_inode_ = sb->lba_super_block_
        + FS_LAYOUT_SECTORS_SUPERBLOCK;
    sb->lba_map_free_ = sb->lba_map_inode_
        + FS_LAYOUT_SECTORS_MAP_INODES;
    sb->lba_inodes_ = sb->lba_map_free_
        + FS_LAYOUT_SECTORS_MAP_FREE;
    sb->lba_root_ = sb->lba_inodes_ + MAX_INODE;
    sb->lba_free_ = sb->lba_root_ + FS_LAYOUT_SECTORS_ROOT;
}

/**
 * @brief Set up inode metadata
 * 
 * @param necessary_sectors 
 * @param dev_sectors 
 */
void
setup_inode(uint32_t necessary_sectors, uint32_t dev_sectors) {

    // - for direct, all 8 bytes of inode block are direct
    // - for single indirect, the previous 6 bytes are direct
    // and the later 2 bytes single indirect
    // - for double indirect, the previous 6 bytes are direct,
    // the following one is single indirect, the last one is
    // double indirect

    const uint32_t MAX_DIRECT = 6,
        ITEM_AMOUNT_A_SECTOR = BYTES_SECTOR / sizeof(lba_index_t);
    const uint32_t SEC_DIRECT =
        MAX_INODE * MAX_INODE_BLOCKS + necessary_sectors;
    const uint32_t SEC_INDIRECT1 =
        MAX_INODE * (MAX_DIRECT + ITEM_AMOUNT_A_SECTOR
        + ITEM_AMOUNT_A_SECTOR) + necessary_sectors;
    const uint32_t SEC_INDIRECT2 =
        MAX_INODE * (MAX_DIRECT + ITEM_AMOUNT_A_SECTOR
        + ITEM_AMOUNT_A_SECTOR * ITEM_AMOUNT_A_SECTOR)
        + necessary_sectors;

    if (dev_sectors < SEC_DIRECT)
        panic("setup_inode_level()");
    else if (SEC_DIRECT <= dev_sectors && dev_sectors < SEC_INDIRECT1)
        __fs_inode_level = INODE_LEVEL0;
    else if (SEC_INDIRECT1 <= dev_sectors && dev_sectors < SEC_INDIRECT2)
        __fs_inode_level = INODE_LEVEL1;
    else    __fs_inode_level = INODE_LEVEL2;

}

/**
 * @brief Set up the disk
 */
void
setup_disk_layout(void) {
    /************************************************************************************
     * DISK LAYOUT :                                                                    *
     * ┌──────────┬─────────────┬────────────┬──────────┬────────────┬──────────┬──────┐*
     * │ RESERVED │ SUPER_BLOCK │ MAP_INODES │ MAP_FREE │   INODES   │   ROOT   │ FREE │*
     * └──────────┴─────────────┴────────────┴──────────┴────────────┴──────────┴──────┘*
     * |          \             \            \          \            \          \       *
     * | 1 sector  \  2 sectors  \  1 sector  \ 1 sector \ 64 sectors \ 1 sector \      *
     *                                                                                  *
     * NOTE : the first sector will, if necessary, use for Partition                    *
     ************************************************************************************/

    uint32_t necessary_sectors =
        FS_LAYOUT_SECTORS_PARTITION
        + FS_LAYOUT_SECTORS_SUPERBLOCK
        + FS_LAYOUT_SECTORS_MAP_INODES
        + FS_LAYOUT_SECTORS_MAP_FREE
        + MAX_INODE
        + FS_LAYOUT_SECTORS_ROOT;
    uint32_t dev_sectors =
        ata_space.device_info_[ata_space.current_select_].total_sectors_;

    super_block_t super_block;
    bzero(&super_block, sizeof(super_block_t));
    setup_super_block(&super_block, necessary_sectors, dev_sectors);
    setup_inode(necessary_sectors, dev_sectors);
}
