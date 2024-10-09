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
 * @brief whether the dir is root dir
 * 
 * @param dir the specific directory
 * @retval true: is the root
 * @retval false: not the root
 */
bool
is_root_dir(const char *dir) {
    if (dir == null)    panic("is_root_dir(): null pointer");

    static char ROOT_DIR[DIRITEM_NAME_LEN];
    static bool is_init = false;
    if (!is_init) {
        memmove(ROOT_DIR, DIRNAME_ROOT_STR, 1);
        ROOT_DIR[1] = 0;
        is_init = true;
    }
    return strcmp(dir, ROOT_DIR);
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
    if (strlen(d1->name_) != strlen(d2->name_))    return false;
    return (d1->type_ == d2->type_
        && strcmp(d1->name_, d2->name_));
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
    if (block == null)    panic("diritem_read(): null pointer");
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
 * @brief find the specific dir item
 * 
 * @param dir   the absolute path to be searched
 * @param found diritem object found
 * 
 * @retval true: found
 * @retval false: not found
 */
bool
diritem_find(const char *dir, diritem_t *found) {
    if (found == null)    panic("diritem_find(): null pointer");
    bzero(found, sizeof(diritem_t));

    static const char SEPARATE = '/';
    if (dir && dir[0] != SEPARATE)
        panic("diritem_find(): not absolute path");

    diritem_t cur;
    memmove(&cur, &__fs_root_dir, sizeof(diritem_t));

    char name_storage[DIRITEM_NAME_LEN] = { 0 };
    uint32_t count = 0;
    for (uint32_t i = 0; i < strlen(dir); ++i)
        if (dir[i] == SEPARATE)    ++count;

    bool fexit_in_advance = false;
    if (is_root_dir(dir) == false) {
        dirblock_t *dirblock = dyn_alloc(sizeof(dirblock_t));
        for (uint32_t i = 1; i <= count; ++i) {
            // get the current directory name
            //   e.g. "/dir1/dir2/dir3" -> "dir3"
            //                    ^
            //                    |
            //                    dir pointer
            bzero(name_storage, sizeof(name_storage));
            strsep(dir, SEPARATE, i, name_storage);
            if (name_storage[0] == '\0') {
                fexit_in_advance = false;
                break;
            }

            // traversal inode blocks
            diritem_t *temp = null;
            uint32_t j = 0;
            bool ffound = false;
            for (; j < __super_block.inode_block_index_max_; ++j) {
                // the block filled with directory items
                lba_index_t lba = iblock_get(cur.inode_idx_, j);

                // assure that the later blocks all invalid
                // if meet the first invalid block
                if (lba < __super_block.lba_free_)    break;

                free_rw_disk(dirblock, lba, ATA_CMD_IO_READ);
                for (uint32_t k = 0; k < dirblock->amount_; ++k) {
                    temp = dirblock->dir_ + k;
                    if (strcmp(temp->name_, name_storage)) {
                        memmove(&cur, temp, sizeof(diritem_t));
                        ffound = true;
                        break;
                    }
                } // end for(k)

                if (ffound)    break;
            } // end for(j)

            if (ffound == false) {
                fexit_in_advance = true;
                break;
            }

        } // end for(i)

        dyn_free(dirblock);
    }

    if (fexit_in_advance) {
        bzero(found, sizeof(diritem_t));
        return false;
    } else {
        memmove(found, &cur, sizeof(diritem_t));
        return true;
    }
}

/**
 * @brief get a new dirblock
 * 
 * @param result new dirblock
 * @param self   inode index itself
 * @param parent parent inode index
 */
void
dirblock_get_new(dirblock_t *result, idx_t self, idx_t parent) {
    if (result == null)    panic("dirblock_get_new(): null pointer");

    // [0] .
    // [1] ..
    diritem_t *cur = result->dir_, *pre = result->dir_ + 1;
    cur->type_ = pre->type_ = INODE_TYPE_DIR;
    cur->inode_idx_ = self;
    pre->inode_idx_ = parent;
    bzero(cur->name_, DIRITEM_NAME_LEN);
    bzero(pre->name_, DIRITEM_NAME_LEN);
    memmove(cur->name_, DIR_CUR, strlen(DIR_CUR));
    memmove(pre->name_, DIR_PRE, strlen(DIR_PRE));
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
    diritem_set(&__fs_root_dir, INODE_TYPE_DIR, INODE_INDEX_ROOT, DIRNAME_ROOT_STR);

    if (is_new) {

        dirblock_t dirblock;
        bzero(&dirblock, sizeof(dirblock_t));
        dirblock_get_new(&dirblock, INODE_INDEX_ROOT, INVALID_INDEX);

        // setup inode
        lba_index_t free_block = free_allocate();
        inode_set(INODE_INDEX_ROOT, 1, free_block);
        inodes_rw_disk(INODE_INDEX_ROOT, ATA_CMD_IO_WRITE);
        inode_map_setup(INODE_INDEX_ROOT, true);

        free_map_setup(free_block, true);
        free_rw_disk(&dirblock, free_block, ATA_CMD_IO_WRITE);

        inode_map_update();
        free_map_update();
    }
}

/**
 * @brief get current directory
 * 
 * @param buff    buffer of the current directory
 * @param bufflen size of the buffer
 * 
 * @retval 0: succeed
 * @retval -1: failed, and the buffer will be fill in zero
 */
int
dir_get_current(char *buff, uint32_t bufflen) {
    pcb_t *cur_pcb = get_current_pcb();
    char *cur_dir = null;
    uint32_t acc = 0;

    for (idx_t i = 0; i < MAX_OEPN_DIR; ++i) {
        cur_dir = cur_pcb->dir_ + i * DIRITEM_NAME_LEN;
        if (cur_dir[0] == 0)    break;

        uint32_t len = strlen(cur_dir);
        if (acc + len > bufflen) {
            bzero(buff, bufflen);
            return -1;
        } else {
            memmove(buff + acc, cur_dir, len);
            acc += len;
        }
    }

    buff[acc] = 0;
    return 0;
}

/**
 * @brief change the current directory
 * 
 * @param dir given a directory to change
 * 
 * @retval 0: change succeed
 * @retval -1: change failed, no such directory
 * @retval -2: change failed, the given path is a file
 */
int
dir_change(const char *dir) {
    pcb_t *cur_pcb = get_current_pcb();
    if (dir == null) {
        // change to root directory when the given dir is null
        char *cur_dir = cur_pcb->dir_;
        bzero(cur_dir, cur_pcb->dirlen_);
        cur_dir[0] = DIRNAME_ROOT_ASCII;
        cur_dir[1] = 0;
        return 0;
    }

    diritem_t *cur_diritem = dyn_alloc(sizeof(diritem_t));
    bzero(cur_diritem, sizeof(diritem_t));
    char *dir_buff = dyn_alloc(PGSIZE);
    bzero(dir_buff, PGSIZE);
    if (dir[0] != DIRNAME_ROOT_ASCII) {
        dir_get_current(dir_buff, PGSIZE);
        memmove(dir_buff + strlen(dir_buff), dir, strlen(dir));
    } else    memmove(dir_buff, dir, strlen(dir));
    if (diritem_find(dir_buff, cur_diritem) == false) {
        dyn_free(dir_buff);
        dyn_free(cur_diritem);
        return -1;
    }
    dyn_free(dir_buff);

    if (cur_diritem->type_ == INODE_TYPE_FILE) {
        dyn_free(dir_buff);
        dyn_free(cur_diritem);
        return -2;
    }

    char *cur_dir = cur_pcb->dir_;
    if (dir[0] == DIRNAME_ROOT_ASCII) {
        char name_storage[DIRITEM_NAME_LEN] = { 0 };

        // something not good is that `strsep()` must begins at index 1
        //   so we MUST invidially handle index 0, the root directory
        cur_dir[0] = DIRNAME_ROOT_ASCII;
        cur_dir[1] = 0;

        idx_t i = 1;
        for (; i < MAX_OEPN_DIR; ++i) {
            cur_dir += i * DIRITEM_NAME_LEN;

            bzero(name_storage, sizeof(name_storage));
            strsep(dir, DIRNAME_ROOT_ASCII, i, name_storage);
            if (name_storage[0] == 0)    break;

            uint32_t len = strlen(name_storage);
            name_storage[len] = DIRNAME_ROOT_ASCII;
            name_storage[len + 1] = 0;
            memmove(cur_dir, name_storage, sizeof(name_storage));
        } // end for()
        if (i == MAX_OEPN_DIR)    panic("dir_change(): working dir overflow");

    } else {
        idx_t i = 1;
        for (; i < MAX_OEPN_DIR; ++i) {
            cur_dir += i * DIRITEM_NAME_LEN;
            if (cur_dir[0] == 0) {
                uint32_t len = strlen(dir);
                memmove(cur_dir, dir, len);
                cur_dir[len] = DIRNAME_ROOT_ASCII;
                cur_dir[len + 1] = 0;
                break;
            }
        } // end for()
        if (i == MAX_OEPN_DIR)    panic("dir_change(): bug");
    }

    dyn_free(cur_diritem);
    return 0;
}
