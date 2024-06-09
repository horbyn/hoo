/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "dir.h"

static char *__fs_dir_basic[] = { ".", "..", 0 };
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
 * @param dir the absolute path to be searched
 * @return diritem object
 */
diritem_t *
diritem_find(const char *dir) {
    static char separate = '/';
    if (dir && dir[0] != separate)
        panic("diritem_find(): not absolute path");

    diritem_t *ret = null;
    if (dir) {
        diritem_t *worker = &__fs_root_dir;
        char name_storage[DIRITEM_NAME_LEN] = { 0 };

        ++dir;
        while (dir) {
            // get the current directory name
            //   e.g. "/dir1/dir2/dir3" -> "dir3"
            //                    ^
            //                    |
            //                    dir pointer
            bzero(name_storage, sizeof(name_storage));
            uint32_t pos = 0;
            while (dir[pos] && dir[pos] != separate)    ++pos;
            if (pos >= DIRITEM_NAME_LEN)
                panic("diritem_find(): invalid dir name");

            memmove(name_storage, dir, pos);
            name_storage[pos] = 0;

            for (uint32_t i = 0; i <= pos + 1; ++i, ++dir)
                if (dir[i] == 0)    break;

            // traversal inode blocks
            dirblock_t *dirblock = dyn_alloc(sizeof(dirblock_t));
            uint32_t i = 0;
            for (; i < __super_block.inode_block_index_max_; ++i) {
                // the block filled with directory items
                inode_retrieve_block(dirblock, worker->inode_idx_, i);

                bool found = false;
                for (uint32_t j = 0; j < dirblock->amount_; ++j) {
                    ret = dirblock->dir_ + j;
                    if (memcmp(ret->name_, name_storage, pos) == 0) {
                        found = true;
                        break;
                    }
                } // end for(j)

                if (found)    break;
            } // end for(i)
            dyn_free(dirblock);

            if (i == __super_block.inode_block_index_max_)
                panic("diritem_find(): no such directory item");
            else    break;

        } // end while(dir)
    }

    return ret;
}

/**
 * @brief directory block writes to disk
 * 
 * @param db  dirblock object
 * @param lba lba index
 * @param cmd ata command
 */
void
dirblock_rw(dirblock_t *db, lba_index_t lba, ata_cmd_t cmd) {
    if (db == null)    panic("dirblock_rw(): null pointer");
    if (lba == INVALID_INDEX)    panic("dirblock_rw(): invalid lba index");

    ata_driver_rw(db, sizeof(dirblock_t), lba, cmd);
}

/**
 * @brief Set up the root dir
 * 
 * @param is_new new disk
 */
void
setup_root_dir(bool is_new) {

    // setup dir item
    diritem_set(&__fs_root_dir, INODE_TYPE_DIR, INODE_INDEX_ROOT, "/");

    if (is_new) {

        dirblock_t dirblock;
        bzero(&dirblock, sizeof(dirblock_t));

        // setup inode
        lba_index_t free_block = free_allocate();
        inode_set(INODE_INDEX_ROOT, 0, free_block);

        // setup block
        diritem_t dir_cur, dir_pre;
        diritem_set(&dir_cur, INODE_TYPE_DIR,
            INODE_INDEX_ROOT, __fs_dir_basic[FS_DIR_BASIC_CUR]);
        diritem_set(&dir_pre, INODE_TYPE_DIR, INVALID_INDEX,
            __fs_dir_basic[FS_DIR_BASIC_PRE]);

        // NEED NOT check return value because it is the first item
        diritem_write(&dirblock, &dir_cur);
        diritem_write(&dirblock, &dir_pre);
        dirblock_rw(&dirblock, free_block, ATA_CMD_IO_WRITE);
    }
}
