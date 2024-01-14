/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "inodes.h"

static uint8_t __fs_map_inodes[BYTES_SECTOR];               // inodes map in-memory c
inode_t __fs_inodes[MAX_INODES];                            // inodes in-memory cache

/**
 * @brief allocate inode (only for index, not involves disk rw)
 * 
 * @return index of inode array
 */
idx_t
inode_allocate() {

    uint32_t len = BYTES_SECTOR *
        (__super_block.lba_inodes_ - __super_block.lba_map_inode_);

    return bitmap_scan(__fs_map_inodes, len);
}

/**
 * @brief release inode (only for index, not involves disk rw)
 * 
 * @param idx index of inode array
 */
void
inode_release(idx_t idx) {
    if (idx == INVALID_INDEX)
        panic("inode_release(): invalid index");

    bitmap_clear(__fs_map_inodes, idx);
}

/**
 * @brief fill in inode structure
 * 
 * @param inode_idx inode index
 * @param size blocks size
 * @param base_lba blocks where it begins
 */
void
set_inode(idx_t inode_idx, size_t size, lba_index_t base_lba) {
    if (inode_idx == INVALID_INDEX)
        panic("set_inode(): invalid inode");
    if (base_lba < __super_block.lba_blocks_)
        panic("set_inode(): invalid lba");

    inode_t *inode = &__fs_inodes[inode_idx];
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
inodes_rw_disk(idx_t inode_idx, ata_cmd_t cmd) {
    if (inode_idx == INVALID_INDEX)
        panic("inodes_write_to_disk(): invalid index");
    if (cmd == ATA_CMD_IO_READ
        && bitmap_test(__fs_map_inodes, inode_idx) == false)
        panic("inodes_read_from_disk(): not allow to read from an empty inode");

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    memmove(sect, &(__fs_inodes[inode_idx]), sizeof(inode_t));
    ata_driver_rw(sect, sizeof(sect),
        __super_block.lba_inodes_ + inode_idx, cmd);

    if (cmd == ATA_CMD_IO_WRITE)
        bitmap_set(__fs_map_inodes, inode_idx);
    else    bitmap_clear(__fs_map_inodes, inode_idx);
    ata_driver_rw(__fs_map_inodes, sizeof(__fs_map_inodes),
        __super_block.lba_map_inode_, cmd);
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
    if (is_new == false) {
        for (size_t i = 0; i < MAX_INODES; ++i) {
            if (bitmap_test(__fs_map_inodes, i) == true)
                inodes_rw_disk(i, cmd);
        }
    }
}
