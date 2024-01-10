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

    uint32_t len = BYTES_SECTOR *
        (__super_block.lba_inodes_ - __super_block.lba_map_inode_);

    uint32_t bit = bitmap_scan(__fs_map_inodes, len);
    return (__super_block.lba_inodes_ + bit);
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
inode_rw_disk(idx_t inode_idx, ata_cmd_t cmd) {

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    memmove(sect, &(__fs_inodes[inode_idx]), sizeof(inode_t));

    ata_driver_rw(sect, sizeof(sect),
        __super_block.lba_inodes_ + inode_idx, cmd);
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
    ata_driver_rw(__fs_map_inodes, sizeof(__fs_map_inodes),
        FS_LAYOUT_BASE_MAP_INODES, cmd);

    // inodes layout
    for (size_t i = 0; i < MAX_INODES; ++i)
        inode_rw_disk(i, cmd);
}
