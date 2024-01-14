/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "super_block.h"

static enum_index_level_t __fs_level;
static uint32_t __fs_bitmap_free_sectors;
super_block_t __super_block;

static void
init_layout(void) {
    uint32_t dev_sec =
        ata_space.device_info_[ata_space.current_select_].total_sectors_;

    const uint32_t MAX_DIRECT = 6,
        ITEMS_PER_SECTOR = BYTES_SECTOR / sizeof(lba_index_t);

#define FUNC_GET_FREE_BITMAP(free) \
    (((free) / BITS_PER_BYTE / BYTES_SECTOR) + 1)

    // For direct, all 8 bytes of inode block are direct.
    //   So a file occupies `MAX_INODE_BLOCKS(8)` sectors.
    //   Its bitmap occupies "`FREE_BITMAP_SEC_0`" sectors
    const uint32_t FREE_SEC_0 = MAX_INODES * MAX_INODE_BLOCKS,
        FREE_BITMAP_SEC_0 = FUNC_GET_FREE_BITMAP(FREE_SEC_0),
        SEC_DIRECT = FS_LAYOUT_BASE_MAP_FREE + FREE_BITMAP_SEC_0 + FREE_SEC_0;

    // For single indirect, the previous 6 bytes are direct
    //   and the later 2 bytes single indirect. So a file occupies
    //   "`MAX_DIRECT`(6) + `ITEMS_PER_SECTOR`(128) * 2" sectors.
    //   Its bitmap as same as above
    const uint32_t FREE_SEC_1 =
        MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR * 2),
        FREE_BITMAP_SEC_1 = FUNC_GET_FREE_BITMAP(FREE_SEC_1),
        SEC_INDIRECT1 = FS_LAYOUT_BASE_MAP_FREE + FREE_BITMAP_SEC_1 + FREE_SEC_1;

    // For double indirect, the previous 6 bytes are direct,
    //   the following one is single indirect, the last one is
    //   double indirect. So a file occupies "`MAX_DIRECT`(6) 
    //   + `ITEMS_PER_SECTOR`(128) + `ITEMS_PER_SECTOR`^2 (128^2)"
    //   sectors. Its bitmap as same as above
    const uint32_t FREE_SEC_2 =
        MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR
        + ITEMS_PER_SECTOR * ITEMS_PER_SECTOR),
        FREE_BITMAP_SEC_2 = FUNC_GET_FREE_BITMAP(FREE_SEC_2),
        SEC_INDIRECT2 = FS_LAYOUT_BASE_MAP_FREE + FREE_BITMAP_SEC_2 + FREE_SEC_2;

    if (dev_sec < SEC_DIRECT)
        panic("setup_inode_level()");
    else if (SEC_DIRECT <= dev_sec && dev_sec < SEC_INDIRECT1) {
        __fs_bitmap_free_sectors = FREE_BITMAP_SEC_0;
        __fs_level = INDEX_LEVEL0;
    } else if (SEC_INDIRECT1 <= dev_sec && dev_sec < SEC_INDIRECT2) {
        __fs_bitmap_free_sectors = FREE_BITMAP_SEC_1;
        __fs_level = INDEX_LEVEL1;
    } else {
        __fs_bitmap_free_sectors = FREE_BITMAP_SEC_2;
        __fs_level = INDEX_LEVEL2;
    }
}

/**
 * @brief Set up the super block
 * 
 * @retval true: a new disk
 * @retval false: an existed disk
 */
bool
setup_super_block() {

    bool is_new = false;

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    ata_driver_rw(sect, sizeof(sect),
        FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_READ);
    memmove(&__super_block, sect, sizeof(super_block_t));

    if (__super_block.magic_ != FS_HOO_MAGIC) {
        init_layout();

        __super_block.magic_           = FS_HOO_MAGIC;
        __super_block.lba_partition_   = FS_LAYOUT_BASE_PARTITION;
        __super_block.lba_super_block_ = FS_LAYOUT_BASE_SUPERBLOCK;
        __super_block.lba_map_inode_   = FS_LAYOUT_BASE_MAP_INODES;
        __super_block.lba_inodes_      = FS_LAYOUT_BASE_INODES;
        __super_block.lba_map_blocks_    = FS_LAYOUT_BASE_MAP_FREE;
        __super_block.lba_blocks_        = FS_LAYOUT_BASE_MAP_FREE
            + __fs_bitmap_free_sectors;
        __super_block.index_level_     = __fs_level;

        memmove(sect, &__super_block, sizeof(super_block_t));
        ata_driver_rw(sect, sizeof(sect),
            FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_WRITE);

        is_new = true;
    }

    return is_new;
}
