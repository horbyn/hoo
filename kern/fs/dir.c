/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "dir.h"

static char *__fs_dir_basic[] = { ".", "..", 0 };
dir_item_t __fs_root_dir;

/**
 * @brief fill in the `dir_item_t` structure
 * 
 * @param dir   the dir structure to be filled in
 * @param type  inode type
 * @param inode_idx inode index
 * @param name  name corresponding to this dir
 */
void
set_dir_item(dir_item_t *dir, enum_inode_type type, idx_t inode_idx,
const char *name) {

    if (name == null)    panic("set_dir_item(): invalid dir name");

    dir->type_      = type;
    dir->inode_idx_ = inode_idx;
    memmove(dir->name_, name, strlen(name) + 1);
}

void
set_dir_block(dir_block_t *blocks, size_t block_size,
const dir_item_t *items, uint32_t item_amount) {
    if (blocks == null)    panic("set_dir_block(): invalid blocks");
    if (items == null || item_amount == 0)    return;

    /*
     * the dir block array like below:
     * ┌───────────┬────────────┐ ┌───────────┬────────────┐
     * │ amount(x) │ dir_item_t │-│ amount(0) │ dir_item_t │- ...
     * └───────────┴────────────┘ └───────────┴────────────┘
     */
}

/**
 * @brief directory items writes to disk
 * 
 * @param dest destination of directory item
 * @param to_write directory items to write
 * @param amount directory amounts
 */
void
dirs_write_disk(dir_item_t *dest, dir_item_t *to_write, uint32_t amount) {
    if (dest == null)    panic("dirs_rw_disk(): invalid dir");
    if (to_write == null)    return;

    /*
     * the directory items in disk like below:
     *
     * ┌────────┬─────────────────────────────┐
     * │ amount │ (21) `dir_item_t` structure │
     * └────────┴─────────────────────────────┘
     */

    uint32_t need_size = __fs_inodes[dest->inode_idx_].size_ + amount;
    const uint32_t MAX_DIRECT = 6,
        ITEMS_PER_SECTOR = BYTES_SECTOR / sizeof(lba_index_t);
    const uint32_t LEVEL0_DIR = MAX_INODE_BLOCKS * MAX_DIRITEM_PER_BLOCK,
        LEVEL1_DIR = (MAX_DIRECT + ITEMS_PER_SECTOR * 2) * MAX_DIRITEM_PER_BLOCK,
        LEVEL2_DIR = (MAX_DIRECT + ITEMS_PER_SECTOR +
            ITEMS_PER_SECTOR * ITEMS_PER_SECTOR) * MAX_DIRITEM_PER_BLOCK;
    if ((__super_block.index_level_ == INDEX_LEVEL0 && need_size > LEVEL0_DIR)
        || (__super_block.index_level_ == INDEX_LEVEL1 && need_size > LEVEL1_DIR)
        || (__super_block.index_level_ == INDEX_LEVEL2 && need_size > LEVEL2_DIR))
        panic("dirs_rw_disk(): no enough space in inode");

    dir_block_t block;
    bool fchange = false;
    for (size_t i = 0; amount != 0; ++i) {
        if (i == MAX_INODE_BLOCKS)    break;
        bzero(&block, sizeof(dir_block_t));

        lba_index_t lba = __fs_inodes[dest->inode_idx_].iblocks_[i];
        if (lba < __super_block.lba_free_)
            panic("dirs_rw_disk(): invalid lba");
        // dir item needs not to read when `inode.size_` is zero,
        //   because it is a new dir
        if (__fs_inodes[dest->inode_idx_].size_ != 0)
            ata_driver_rw(&block, sizeof(dir_block_t), lba, ATA_CMD_IO_READ);

        for (; block.amount_ < MAX_DIRITEM_PER_BLOCK;) {
            memmove(&block.dir_[block.amount_], to_write, sizeof(dir_item_t));
            ++block.amount_;
            ++to_write;
            ++__fs_inodes[dest->inode_idx_].size_;
            fchange = true;
            --amount;
            if (amount == 0)    break;
        } // end for(dir_block)

        if (fchange) {
            ata_driver_rw(&block, sizeof(dir_block_t), lba, ATA_CMD_IO_WRITE);
            fchange = false;
        }
    } // end for(amount)

    inodes_rw_disk(dest->inode_idx_, ATA_CMD_IO_WRITE);
}

/**
 * @brief Set up the root dir
 * 
 * @param is_new new disk
 */
void
setup_root_dir(bool is_new) {

    // setup dir item
    set_dir_item(&__fs_root_dir, INODE_TYPE_DIR,
        INODE_INDEX_ROOT, "/");

    if (is_new) {

        uint8_t sec[BYTES_SECTOR];
        bzero(sec, sizeof(sec));

#define FUNC_GET_DIR_SIZE(item_amount) \
        (item_amount)

        // setup inode
        lba_index_t free_block = free_allocate();
        set_inode(INODE_INDEX_ROOT, 0, free_block);

        // setup block
        dir_item_t dir_cur, dir_pre;
        set_dir_item(&dir_cur, INODE_TYPE_DIR,
            INODE_INDEX_ROOT, __fs_dir_basic[FS_DIR_BASIC_CUR]);
        set_dir_item(&dir_pre, INODE_TYPE_DIR, INVALID_INDEX,
            __fs_dir_basic[FS_DIR_BASIC_PRE]);

        memmove(sec, &dir_cur, sizeof(dir_item_t));
        memmove(sec + sizeof(dir_item_t), &dir_pre, sizeof(dir_item_t));
        dirs_write_disk(&dir_cur, (dir_item_t *)sec, NELEMS(__fs_dir_basic) - 1);
    }
}
