/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "super_block.h"

super_block_t __super_block;

/**
 * @brief file system initialization
 * @param sb super block
 */
static void
init_layout(super_block_t *sb) {
    if (sb == null)    panic("init_layout(): null pointer");
    ataspc_t *ata_space = get_ataspace();
    uint32_t dev_sec =
        ata_space->device_info_[ata_space->current_select_].total_sectors_;

#define FUNC_GET_FREE_BITMAP(free) \
    (((free) / BITS_PER_BYTE / BYTES_SECTOR) + 1)

    // For double indirect, the previous 6 bytes are direct,
    //   the following one is single indirect, the last one is
    //   double indirect. So a file occupies "`MAX_DIRECT`(6) 
    //   + `LBA_ITEMS_PER_SECTOR`(128) + `LBA_ITEMS_PER_SECTOR`^2 (128^2)"
    //   sectors. Its bitmap as same as above
    const uint32_t FREE_SEC_2 =
        MAX_INODES * (MAX_DIRECT + LBA_ITEMS_PER_SECTOR
        + LBA_ITEMS_PER_SECTOR * LBA_ITEMS_PER_SECTOR),
        FREE_BITMAP_SEC_2 = FUNC_GET_FREE_BITMAP(FREE_SEC_2),
        SEC_INDIRECT2 = FS_LAYOUT_BASE_MAP_FREE + FREE_BITMAP_SEC_2 + FREE_SEC_2;

    if (dev_sec < SEC_INDIRECT2) {
        panic("init_layout(): no enough space for file system");
    } else {
        sb->map_free_sectors_ = FREE_BITMAP_SEC_2;
        uint32_t cnt = MAX_INODE_BLOCKS - MAX_DIRECT - 1;
        uint32_t cube = 0;
        for (; cnt > 0; --cnt)
            cube += LBA_ITEMS_PER_SECTOR * LBA_ITEMS_PER_SECTOR;
        sb->inode_block_index_max_ = MAX_DIRECT + LBA_ITEMS_PER_SECTOR + cube;
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
    ata_driver_rw(sect, sizeof(sect), FS_LAYOUT_BASE_SUPERBLOCK, ATA_CMD_IO_READ);
    memmove(&__super_block, sect, sizeof(super_block_t));

    if (__super_block.magic_ != FS_HOO_MAGIC) {
        bzero(&__super_block, sizeof(super_block_t));
        init_layout(&__super_block);

        __super_block.magic_           = FS_HOO_MAGIC;
        __super_block.lba_partition_   = FS_LAYOUT_BASE_PARTITION;
        __super_block.lba_super_block_ = FS_LAYOUT_BASE_SUPERBLOCK;
        __super_block.lba_map_inode_   = FS_LAYOUT_BASE_MAP_INODES;
        __super_block.lba_inodes_      = FS_LAYOUT_BASE_INODES;
        __super_block.lba_map_free_    = FS_LAYOUT_BASE_MAP_FREE;
        __super_block.lba_free_        = __super_block.lba_map_free_
            + __super_block.map_free_sectors_;

        memmove(sect, &__super_block, sizeof(super_block_t));
        ata_driver_rw(sect, sizeof(sect), FS_LAYOUT_BASE_SUPERBLOCK,
            ATA_CMD_IO_WRITE);

        is_new = true;
    }

    return is_new;
}
