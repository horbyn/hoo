/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "dir.h"

static diritem_t __fs_root_dir;

/**
 * @brief fill in the `diritem_t` structure
 * 
 * @param dir   the dir structure to be filled in
 * @param type  inode type
 * @param inode_idx inode index
 * @param name  name corresponding to this dir
 */
void
diritem_set(diritem_t *dir, enum_inode_type type, idx_t inode_idx,
const char *name) {

    if (name == null)    panic("diritem_set(): invalid dir name");

    dir->type_      = type;
    dir->inode_idx_ = inode_idx;
    memmove(dir->name_, name, strlen(name) + 1);
}

/**
 * @brief put the diritem object to dirblock
 * 
 * @param block dirblock
 * @param item  diritem
 * @retval 0:  succeeded
 * @retval -1: failed
 */
int
diritem_write(dirblock_t *block, const diritem_t *item) {
    if (block == null)    panic("diritem_write(): null pointer");
    if (item == null)    return 0;

    // -1, not available space
    if (block->amount_ >= MAX_DIRITEM_PER_BLOCK)    return -1;
    else {
        memmove(&block->dir_[block->amount_], item, sizeof(diritem_t));
        ++block->amount_;
    }
    return 0;
}

/**
 * @brief diritem comparasion
 * 
 * @param d1 diritem 1
 * @param d2 diritem 2
 * @retval true:  same
 * @retval false: not same
 */
static bool
diritem_compare(const diritem_t *d1, const diritem_t *d2) {
    if (d1 == null || d2 == null)    panic("diritem_compare(): null pointer");
    return (d1->type_ == d2->type_
        && memcmp(d1->name_, d2->name_, DIRITEM_NAME_LEN));
}

/**
 * @brief retrieve the diritem object from dirblock
 * 
 * @param block dirblock
 * @param item  diritem
 * @return diritem object
 */
diritem_t *
diritem_read(dirblock_t *block, const diritem_t *item) {
    diritem_t *ret = null;
    if (block == null)    panic("diritem_write(): null pointer");
    if (item == null)    return ret;

    for (uint32_t i = 0; i < block->amount_; ++i) {
        if (diritem_compare(&block->dir_[i], item)) {
            ret = &block->dir_[i];
            break;
        }
    }

    return ret;
}

/**
 * @brief find the specific diritem
 * 
 * @param dir   the absolute path to be searched
 * @param found diritem object found
 */
void
diritem_find(const char *dir, diritem_t *found) {
    static const char SEPARATE = '/';
    if (dir && dir[0] != SEPARATE)
        panic("diritem_find(): not absolute path");
    if (found == null)    panic("diritem_find(): null pointer");

    diritem_t *item = &__fs_root_dir;
    if (dir) {
        uint32_t count = 0;
        for (uint32_t i = 0; i < strlen(dir); ++i)
            if (dir[i] == SEPARATE)    ++count;

        diritem_t *worker = &__fs_root_dir;
        char name_storage[DIRITEM_NAME_LEN] = { 0 };

        for (uint32_t i = 1; i <= count; ++i) {
            // get the current directory name
            //   e.g. "/dir1/dir2/dir3" -> "dir3"
            //                    ^
            //                    |
            //                    dir pointer
            bzero(name_storage, sizeof(name_storage));
            strsep(dir, SEPARATE, i, name_storage);

            // traversal inode blocks
            dirblock_t *dirblock = dyn_alloc(sizeof(dirblock_t));
            uint32_t j = 0;
            for (; j < __super_block.inode_block_index_max_; ++j) {
                // the block filled with directory items
                lba_index_t lba = iblock_get(worker->inode_idx_, j);
                free_rw_disk(dirblock, lba, ATA_CMD_IO_READ, false);

                bool found = false;
                for (uint32_t k = 0; k < dirblock->amount_; ++k) {
                    item = dirblock->dir_ + k;
                    if (memcmp(item->name_, name_storage, strlen(name_storage)) == 0) {
                        found = true;
                        break;
                    }
                } // end for(k)

                if (found)    break;
            } // end for(j)
            dyn_free(dirblock);

            if (j == __super_block.inode_block_index_max_)
                panic("diritem_find(): no such directory item");
            else    break;

        } // end for(i)
    }

    memmove(found, item, sizeof(diritem_t));
}

/**
 * @brief get a new dirblock
 * 
 * @param result new dirblock
 */
void
dirblock_get_new(dirblock_t *result) {
    if (result == null)    panic("dirblock_get_new(): null pointer");

    // [0] .
    // [1] ..
    diritem_t *cur = result->dir_, *pre = result->dir_ + 1;
    cur->type_ = pre->type_ = INODE_TYPE_DIR;
    cur->inode_idx_ = INODE_INDEX_ROOT;
    pre->inode_idx_ = INVALID_INDEX;
    bzero(cur->name_, DIRITEM_NAME_LEN);
    bzero(pre->name_, DIRITEM_NAME_LEN);
    memmove(cur->name_, ".", sizeof(1));
    memmove(pre->name_, "..", sizeof(2));
    result->amount_ = 2;
}

/**
 * @brief Set up the root dir
 * 
 * @param is_new new disk
 */
void
setup_root_dir(bool is_new) {

    // setup dir item
    diritem_set(&__fs_root_dir, INODE_TYPE_DIR, INODE_INDEX_ROOT, DIRNAME_ROOT);

    if (is_new) {

        dirblock_t dirblock;
        bzero(&dirblock, sizeof(dirblock_t));
        dirblock_get_new(&dirblock);

        // setup inode
        lba_index_t free_block = free_allocate();
        inode_set(INODE_INDEX_ROOT, dirblock.amount_, free_block);
        inodes_rw_disk(INODE_INDEX_ROOT, ATA_CMD_IO_WRITE);

        free_rw_disk(&dirblock, free_block, ATA_CMD_IO_WRITE, true);
    }
}