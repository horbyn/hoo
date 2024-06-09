/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "inodes.h"

// inodes map in-memory cache (set means caching on disk;
//     clear means caching in memory)
static uint8_t __bmbuff_fs_inodes[BYTES_SECTOR];
// inodes in-memory cache
inode_t __fs_inodes[MAX_INODES];
static bitmap_t __bmfs;

/**
 * @brief allocate inode (only for index, not involves disk rw)
 * 
 * @return index of inode array
 */
idx_t
inode_allocate() {
    return bitmap_scan_empty(&__bmfs);
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

    bitmap_clear(&__bmfs, idx);
}

/**
 * @brief fill in inode structure
 * 
 * @param inode_idx inode index
 * @param size blocks size
 * @param base_lba blocks where it begins
 */
void
inode_set(idx_t inode_idx, uint32_t size, lba_index_t base_lba) {
    if (inode_idx == INVALID_INDEX)
        panic("inode_set(): invalid inode");
    if (base_lba < __super_block.lba_free_)
        panic("inode_set(): invalid lba");

    inode_t *inode = &__fs_inodes[inode_idx];
    bzero(inode, sizeof(inode_t));
    inode->size_ = size;
    inode->iblocks_[0] = base_lba;
}

/**
 * @brief retrieve the block from direct index method
 * 
 * @param block           block buffer
 * @param inode_idx       inode index
 * @param inode_block_idx inode iblock index
 * @return block
 */
static void
retrieve_block_l0(void *block, idx_t inode_idx, idx_t inode_block_idx) {
    lba_index_t lba_idx = __fs_inodes[inode_idx].iblocks_[inode_block_idx];
    ata_driver_rw(block, BYTES_SECTOR, lba_idx, ATA_CMD_IO_READ);
}

/**
 * @brief retrieve the block from single indirect index method
 * 
 * @param block           block buffer
 * @param inode_idx       inode index
 * @param inode_block_idx inode iblock index
 * @return block
 */
static void
retrieve_block_l1(void *block, idx_t inode_idx, idx_t inode_block_idx) {
    static uint32_t BORDER = MAX_DIRECT + LBA_ITEMS_PER_SECTOR,
        BORDER2 = MAX_DIRECT + LBA_ITEMS_PER_SECTOR * 2;
    lba_index_t lba_indirect = 0;

    if (MAX_DIRECT < inode_block_idx) {
        retrieve_block_l0(block, inode_idx, inode_block_idx);
        return;
    } else if (MAX_DIRECT <= inode_block_idx && inode_block_idx < BORDER)
        lba_indirect = MAX_DIRECT;
    else if (BORDER <= inode_block_idx && inode_block_idx < BORDER2)
        lba_indirect = MAX_DIRECT + 1;
    else    panic("retrieve_block_l1(): inode block array out of range");

    lba_index_t *lba_block = dyn_alloc(BYTES_SECTOR);
    ata_driver_rw(lba_block, BYTES_SECTOR, lba_indirect, ATA_CMD_IO_READ);

    uint32_t i = 0;
    for (; i < LBA_ITEMS_PER_SECTOR; ++i)
        if (lba_block[i] == inode_block_idx)    break;
    if (i == LBA_ITEMS_PER_SECTOR)
        panic("retrieve_block_l1(): index of inode block array not found");

    ata_driver_rw(block, BYTES_SECTOR, lba_block[i], ATA_CMD_IO_READ);
    dyn_free(lba_block);
}

/**
 * @brief retrieve the block from double indirect index method
 * 
 * @param block           block buffer
 * @param inode_idx       inode index
 * @param inode_block_idx inode iblock index
 * @return block
 */
static void
retrieve_block_l2(void *block, idx_t inode_idx, idx_t inode_block_idx) {
    static uint32_t BORDER = MAX_DIRECT + LBA_ITEMS_PER_SECTOR,
        BORDER2 = MAX_DIRECT + ((LBA_ITEMS_PER_SECTOR + 1) * LBA_ITEMS_PER_SECTOR);
    lba_index_t lba_indirect1 = 0;

    if (MAX_DIRECT < inode_block_idx) {
        retrieve_block_l0(block, inode_idx, inode_block_idx);
        return;
    } else if (MAX_DIRECT <= inode_block_idx && inode_block_idx < BORDER) {
        retrieve_block_l1(block, inode_idx, inode_block_idx);
        return;
    } else if (BORDER <= inode_block_idx && inode_block_idx < BORDER2)
        lba_indirect1 = MAX_DIRECT + 1;
    else    panic("retrieve_block_l1(): inode block array out of range");

    // get the double indirect index
    lba_index_t *lba_block_l2 = dyn_alloc(BYTES_SECTOR);
    ata_driver_rw(lba_block_l2, BYTES_SECTOR, lba_indirect1, ATA_CMD_IO_READ);
    lba_index_t lba_indirect2 =
        ((inode_block_idx - MAX_DIRECT - LBA_ITEMS_PER_SECTOR)
        + LBA_ITEMS_PER_SECTOR - 1) / LBA_ITEMS_PER_SECTOR;
    lba_index_t lba_indirect = lba_block_l2[lba_indirect2];

    // get the double indirect index
    lba_index_t *lba_block_l1 = dyn_alloc(BYTES_SECTOR);
    ata_driver_rw(lba_block_l1, BYTES_SECTOR, lba_indirect, ATA_CMD_IO_READ);

    uint32_t i = 0;
    for (; i < LBA_ITEMS_PER_SECTOR; ++i)
        if (lba_block_l1[i] == inode_block_idx)    break;
    if (i == LBA_ITEMS_PER_SECTOR)
        panic("retrieve_block_l1(): index of inode block array not found");

    ata_driver_rw(block, BYTES_SECTOR, lba_block_l1[i], ATA_CMD_IO_READ);
    dyn_free(lba_block_l2);
    dyn_free(lba_block_l1);
}

/**
 * @brief retrieve blocks according to the index of the inode iblock array
 * 
 * @param block           block buffer
 * @param inode_idx       inode index
 * @param inode_block_idx inode iblock index
 * @retval null: error happened 
 */
void
inode_retrieve_block(void *block, idx_t inode_idx, idx_t inode_block_idx) {
    if (block == null)    panic("inode_retrieve_block(): null pointer");
    if (inode_idx == INVALID_INDEX)
        panic("inodes_write_to_disk(): invalid inode index");
    if (inode_block_idx >= __super_block.inode_block_index_max_)
        panic("inodes_write_to_disk(): invalid index of inode block array");

    switch (__super_block.index_level_) {
    case INDEX_LEVEL2:
        retrieve_block_l2(block, inode_idx, inode_block_idx); break;
    case INDEX_LEVEL1:
        retrieve_block_l1(block, inode_idx, inode_block_idx); break;
    default: retrieve_block_l0(block, inode_idx, inode_block_idx); break;
    }

}

/**
 * @brief inode reads from / writes to disk
 * 
 * @param inode_idx inode index
 * @param cmd       ATA command
 */
void
inodes_rw_disk(idx_t inode_idx, ata_cmd_t cmd) {
    if (inode_idx == INVALID_INDEX)
        panic("inodes_write_to_disk(): invalid index");
    if (cmd == ATA_CMD_IO_READ
        && bitmap_test(&__bmfs, inode_idx) == false)
        panic("inodes_read_from_disk(): not allow to read from an empty inode");

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    if (cmd == ATA_CMD_IO_WRITE)
        memmove(sect, &(__fs_inodes[inode_idx]), sizeof(inode_t));
    ata_driver_rw(sect, sizeof(sect), __super_block.lba_inodes_ + inode_idx, cmd);
    if (cmd == ATA_CMD_IO_READ)
        memmove(&(__fs_inodes[inode_idx]), sect, sizeof(inode_t));

    if (cmd == ATA_CMD_IO_WRITE)    bitmap_set(&__bmfs, inode_idx);
    else    bitmap_clear(&__bmfs, inode_idx);
    ata_driver_rw(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes),
        __super_block.lba_map_inode_, ATA_CMD_IO_WRITE);
}

/**
 * @brief Set up inode metadata
 *
 * @param is_new a new disk
 */
void
setup_inode(bool is_new) {

    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bitmap_init(&__bmfs, MAX_INODES, __bmbuff_fs_inodes);
    bzero(__fs_inodes, sizeof(__fs_inodes));
    bzero(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes));

    // inode map layout
    ata_driver_rw(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes),
        FS_LAYOUT_BASE_MAP_INODES, cmd);

    // stash all the inode objects
    if (is_new == false) {
        for (uint32_t i = 0; i < MAX_INODES; ++i) {
            if (bitmap_test(&__bmfs, i) == true)
                inodes_rw_disk(i, cmd);
        }
    }
}
