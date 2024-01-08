/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "inodes.h"

static uint8_t __fs_map_inodes[BYTES_SECTOR];               // inodes map in-memory c
inode_t __fs_inodes[MAX_INODES];                            // inodes in-memory cache

lba_index_t
inode_allocate() {
    return null;
}

/**
 * @brief fill in inode structure
 * 
 * @param inode inode to be filled
 * @param size blocks size
 * @param base_lba blocks where it begins
 */
void
set_inode(inode_t *inode, size_t size, lba_index_t base_lba) {
    if (inode == null)    panic("set_inode(): invalid inode");
    if (base_lba < __super_block.lba_free_)
        panic("set_inode(): invalid lba");

    inode->size_ = size;
    size_t i = 0;
    for (; i < MAX_INODE_BLOCKS; ++i) {
        if (inode->iblock_[i] == 0) {
            inode->iblock_[i] = base_lba;
            break;
        }
    }

    if (i == MAX_INODE_BLOCKS)
        panic("set_inode(): no enough iblock space");
}

void
inode_to_disk(lba_index_t lba) {
    atabuff_t atabuff;
    atabuff_set(&atabuff, &__fs_inodes[lba], sizeof(inode_t),
        __super_block.lba_inodes_ + lba, ATA_CMD_IO_WRITE);
    ata_driver_rw(&atabuff);
}

/**
 * @brief Set up inode metadata
 *
 * @param is_new a new disk
 */
void
setup_inode(bool is_new) {

    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_inodes, sizeof(__fs_map_inodes));
    bzero(__fs_inodes, sizeof(__fs_inodes));

    // inode map layout
    atabuff_t ata_buff;
    atabuff_set(&ata_buff, __fs_map_inodes, sizeof(__fs_map_inodes),
        FS_LAYOUT_BASE_MAP_INODES, cmd);
    ata_driver_rw(&ata_buff);

    // inodes layout
    for (size_t i = 0; i < MAX_INODES; ++i) {
        atabuff_set(&ata_buff, &__fs_inodes[i], sizeof(__fs_inodes[i]),
            FS_LAYOUT_BASE_INODES + i, cmd);
        ata_driver_rw(&ata_buff);
    }
}
