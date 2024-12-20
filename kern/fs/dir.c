/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "dir.h"
#include "free.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"
#include "kern/driver/ata/ata_cmd.h"
#include "kern/hoo/thread_curdir.h"
#include "user/lib.h"

/**
 * @brief get a new dirblock
 * 
 * @param result new dirblock
 * @param self   inode index itself
 * @param parent parent inode index
 */
static void
dirblock_get_new(dirblock_t *result, int self, int parent) {
    if (result == null)    panic("dirblock_get_new(): null pointer");

    // [0] .
    // [1] ..
    diritem_t *cur = result->dir_, *pre = result->dir_ + 1;
    cur->type_ = pre->type_ = INODE_TYPE_DIR;
    cur->inode_idx_ = self;
    pre->inode_idx_ = parent;
    bzero(cur->name_, DIRITEM_NAME_LEN);
    bzero(pre->name_, DIRITEM_NAME_LEN);
    memmove(cur->name_, DIR_CUR, 1);
    memmove(pre->name_, DIR_PRE, 2);
}

/**
 * @brief set diritem from the dirblock
 * 
 * @param db          the specific dirblock
 * @param block_index the index of the dirblock array
 * @param item        the item to be set
 */
static void
dirblock_set(dirblock_t *db, uint32_t block_index, const diritem_t *item) {
    if (db == null)    panic("dirblock_set(): null pointer");
    if (block_index >= MAX_DIRITEM_PER_BLOCK)
        panic("dirblock_set(): index of dirblock overflow");

    diritem_t *di = db->dir_ + block_index;
    if (item == null)    bzero(di, sizeof(diritem_t));
    else    memmove(di, item, sizeof(diritem_t));
}

/**
 * @brief get diritem from the dirblock
 * 
 * @param db          the specific dirblock
 * @param block_index the index of the dirblock array
 * @param result      the result
 */
static void
dirblock_get(const dirblock_t *db, uint32_t block_index, diritem_t *result) {
    if (db == null || result == null)    panic("dirblock_get(): null pointer");
    memmove(result, db->dir_ + block_index, sizeof(diritem_t));
}

/**
 * @brief find the specific diritem subroutine
 * 
 * @param di     diritem object
 * @param search the absolute path to be searched
 * @retval -1: not found
 * @retval the rests: the index of the dirblock array
 */
static int
diritem_find_sub(const diritem_t *di, const char *search) {

    dirblock_t db;
    int ret = -1;

    uint32_t iblock_index =
        __fs_inodes[di->inode_idx_].size_ / MAX_DIRITEM_PER_BLOCK;
    for (uint32_t i = 0; i <= iblock_index; ++i) {
        uint32_t lba = iblock_get(di->inode_idx_, i);
        if (lba == 0)    break;

        free_rw_disk(&db, lba, ATA_CMD_IO_READ);
        uint32_t max_diritem_number = i == iblock_index ?
            __fs_inodes[di->inode_idx_].size_ % MAX_DIRITEM_PER_BLOCK
            : MAX_DIRITEM_PER_BLOCK;
        for (uint32_t j = 0; j < max_diritem_number; ++j) {
            if (strcmp(search, db.dir_[j].name_) == true) {
                /*
                 * inode
                 * +--------------------------------+
                 * |size: 23                        |
                 * |         +--------------------+ |
                 * |iblocks: | 326 | 327 | 0 | .. | |
                 * |         +--------------------+ |
                 * +--------------------------------+
                 *              ^
                 *              |
                 *             `i`
                 * 
                 *            dirblock-326          dirblock-327
                 *            +--------------+      +----------------+
                 *            |0|1|2|3| .. |x|      |0   |1   |2| .. |
                 *            +--------------+      +----------------+
                 * logically:  0 1 2 3      x       x+1, x+2, x+3, ..
                 *               ^
                 *               |
                 *              `j`
                 */
                ret = i * MAX_DIRITEM_PER_BLOCK + j;
                break;
            }

        } // end for(diritem)
        if (ret != -1)    break;

    } // end for(dirblock)

    return ret;
}

/**
 * @brief remove directory item recursively (only for index, not involves disk rw)
 * 
 * @param di diritem object to be removed
 */
static void
diritem_remove_sub(diritem_t *di) {
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));

    if (di->type_ == INODE_TYPE_FILE) {
        for (uint32_t i = 0; i < __super_block.inode_block_index_max_; ++i) {
            uint32_t lba = iblock_get(di->inode_idx_, i);
            if (lba != 0)    free_map_setup(lba, false);
            else    break;
        }

    } else if (di->type_ == INODE_TYPE_DIR) {
        for (uint32_t i = 0; i < __fs_inodes[di->inode_idx_].size_; ++i) {
            uint32_t dirblock_lba = iblock_get(di->inode_idx_, i);
            if (dirblock_lba == 0)    break;

            free_rw_disk(db, dirblock_lba, ATA_CMD_IO_READ);
            // skip . and ..
            for (uint32_t j = 0; j < MAX_DIRITEM_PER_BLOCK; ++j) {
                if ((i == 0 && j == 0 )|| (i == 0 && j == 1))    continue;
                if (db->dir_[j].type_ == INODE_TYPE_INVALID)    break;
                else    diritem_remove_sub(db->dir_ + j);
            } // end for(j)
            free_map_setup(dirblock_lba, false);
        } // end for(i)
    } else    panic("diritem_remove_sub(): bug");
    inode_map_setup(di->inode_idx_, false);

    dyn_free(db);
}

/**
 * @brief fill in the `diritem_t` structure
 * 
 * @param dir   the dir structure to be filled in
 * @param type  inode type
 * @param inode_idx inode index
 * @param name  name corresponding to this dir
 */
void
diritem_set(diritem_t *dir, inode_type_t type, int inode_idx, const char *name) {
    if (dir == null || name == 0)    panic("diritem_set(): null pointer");

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
    if (dir == 0)    panic("is_root_dir(): null pointer");

    static char ROOT_DIR[DIRITEM_NAME_LEN];
    static bool is_init = false;
    if (!is_init) {
        memmove(ROOT_DIR, DIRNAME_ROOT_STR, strlen(DIRNAME_ROOT_STR));
        ROOT_DIR[1] = 0;
        is_init = true;
    }
    return strcmp(dir, ROOT_DIR);
}

/**
 * @brief find the specific diritem
 * 
 * @param dir   the absolute path to be searched
 * @param found diritem object found
 * 
 * @retval true: found
 * @retval false: not found
 */
bool
diritem_find(const char *dir, diritem_t *found) {
    if (dir == 0 || (dir != 0 && dir[0] != DIRNAME_ROOT_ASCII))
        panic("diritem_find(): not absolute path");
    if (found == null)    panic("diritem_find(): null pointer");
    else    bzero(found, sizeof(diritem_t));
    if (is_root_dir(dir)) {
        memmove(found, *(get_root_dir()), sizeof(diritem_t));
        return true;
    }

    diritem_t cur;
    memmove(&cur, *(get_root_dir()), sizeof(diritem_t));
    char name_storage[DIRITEM_NAME_LEN] = { 0 };
    uint32_t dir_sz = strlen(dir), i = 1;
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));

    while (i < dir_sz) {
        uint32_t j = 0;
        while (i < dir_sz && dir[i] != DIRNAME_ROOT_ASCII) {
            name_storage[j++] = dir[i++];
        }
        name_storage[j] = 0;
        ++i;

        int result = diritem_find_sub(&cur, name_storage);
        if (result == -1)    break;
        else  {
            uint32_t lba =
                iblock_get(cur.inode_idx_, result / MAX_DIRITEM_PER_BLOCK);
            if (lba < __super_block.lba_free_)    panic("diritem_find(): bug");
            else    free_rw_disk(db, lba, ATA_CMD_IO_READ);

            diritem_t *temp = i >= dir_sz ? found : &cur;
            dirblock_get(db, result % MAX_DIRITEM_PER_BLOCK, temp);
        }

    } // end while()
    dyn_free(db);

    return found->name_[0] == 0 ? false : true;
}

/**
 * @brief diritem traversal
 * 
 * @param dir diritem
 * @return memory by dynamic allocating (NEED TO RELEASE)
 */
char *
diritem_traversal(diritem_t *dir) {

    uint32_t iblock_index =
        __fs_inodes[dir->inode_idx_].size_ / MAX_DIRITEM_PER_BLOCK;
    const uint32_t ret_sz = DIRITEM_NAME_LEN * __fs_inodes[dir->inode_idx_].size_;
    char *buff = dyn_alloc(ret_sz), *ret = buff;
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));

    bzero(buff, ret_sz);
    for (uint32_t i = 0; i <= iblock_index; ++i) {
        uint32_t lba = iblock_get(dir->inode_idx_, i);
        if (lba == 0)    break;

        free_rw_disk(db, lba, ATA_CMD_IO_READ);
        uint32_t max_diritem_number = i == iblock_index ?
            __fs_inodes[dir->inode_idx_].size_ % MAX_DIRITEM_PER_BLOCK
            : MAX_DIRITEM_PER_BLOCK;
        for (uint32_t j = 0; j < max_diritem_number; ++j) {
            memmove(buff, db->dir_[j].name_, DIRITEM_NAME_LEN);
            if (db->dir_[j].type_ == INODE_TYPE_DIR)
                buff[strlen(buff)] = DIRNAME_ROOT_ASCII;
            buff += DIRITEM_NAME_LEN;

        } // end for(diritem)

    } // end for(dirblock)

    dyn_free(db);
    return ret;
}

/**
 * @brief create current directory item
 * 
 * @param type         file type
 * @param item_name    item name
 * @param parent_inode parent inode index
 * 
 * @return diritem object pointer, NEED to release
 */
diritem_t *
diritem_create(inode_type_t type, const char *item_name, int parent_inode) {
    diritem_t *di = dyn_alloc(sizeof(diritem_t));
    bzero(di, sizeof(diritem_t));
    int inode_cur = inode_allocate();
    inode_map_setup(inode_cur, true);
    diritem_set(di, type, inode_cur, item_name);

    // use to initialize inode
    int free = free_allocate();
    free_map_setup(free, true);

    if (type == INODE_TYPE_FILE) {
        inode_set(inode_cur, 0, free);
    } else if (type == INODE_TYPE_DIR) {
        inode_set(inode_cur, 2, free);

        // new diritem push to disk
        dirblock_t *db_new = dyn_alloc(sizeof(dirblock_t));
        bzero(db_new, sizeof(dirblock_t));
        dirblock_get_new(db_new, inode_cur, parent_inode);
        free_rw_disk(db_new, free, ATA_CMD_IO_WRITE);
        free_map_update();
        dyn_free(db_new);
    } else    panic("diritem_create(): bug");

    inodes_rw_disk(inode_cur, ATA_CMD_IO_WRITE);
    inode_map_update();
    return di;
}

/**
 * @brief current diritem push to parent's
 * 
 * @param parent parent diritem
 * @param cur    current diritem
 */
void
diritem_push(diritem_t *parent, diritem_t *cur) {
    if (parent == null || cur == null)    panic("diritem_push(): null pointer");

    // get parent dirblock
    uint32_t index_iblock =
        __fs_inodes[parent->inode_idx_].size_ / MAX_DIRITEM_PER_BLOCK;
    uint32_t block_lba = iblock_get(parent->inode_idx_, index_iblock);
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    if (block_lba >= __super_block.lba_free_)
        free_rw_disk(db, block_lba, ATA_CMD_IO_READ);
    else if (block_lba == 0) {
        bzero(db, sizeof(dirblock_t));
        block_lba = free_allocate();
        free_map_setup(block_lba, true);
        free_map_update();

        iblock_set(parent->inode_idx_, index_iblock, block_lba);
        inodes_rw_disk(parent->inode_idx_, ATA_CMD_IO_WRITE);
    } else    panic("diritem_push(): bug");

    // push to parent
    uint32_t db_index =
        __fs_inodes[parent->inode_idx_].size_ % MAX_DIRITEM_PER_BLOCK;
    memmove(&db->dir_[db_index], cur, sizeof(diritem_t));
    ++__fs_inodes[parent->inode_idx_].size_;
    free_rw_disk(db, block_lba, ATA_CMD_IO_WRITE);
    inodes_rw_disk(parent->inode_idx_, ATA_CMD_IO_WRITE);

    dyn_free(db);
}

/**
 * @brief remove current diritem from parent's
 * 
 * @param parent parent diritem
 * @param cur    current diritem
 * 
 * @retval 0: succeed
 * @retval -1: failed, not such filename
 */
int
diritem_remove(diritem_t *parent, diritem_t *cur) {
    if (parent == null || cur == null)    panic("diritem_remove(): null pointer");

    int result = diritem_find_sub(parent, cur->name_);
    if (result == -1)    return -1;

    uint32_t lba = iblock_get(parent->inode_idx_, result / MAX_DIRITEM_PER_BLOCK);
    if (lba < __super_block.lba_free_)    panic("diritem_remove(): bug");

    // remove from parent
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    free_rw_disk(db, lba, ATA_CMD_IO_READ);
    dirblock_set(db, result % MAX_DIRITEM_PER_BLOCK, null);
    free_rw_disk(db, lba, ATA_CMD_IO_WRITE);
    --__fs_inodes[parent->inode_idx_].size_;
    inodes_rw_disk(parent->inode_idx_, ATA_CMD_IO_WRITE);

    // remove from child
    diritem_remove_sub(cur);
    free_map_update();
    inode_map_update();

    dyn_free(db);
    return 0;
}

/**
 * @brief get the root directory item object
 * 
 * @return root directory
 */
diritem_t **
get_root_dir(void) {
    static diritem_t *__fs_root_dir;
    return &__fs_root_dir;
}

/**
 * @brief change the current directory
 * 
 * @param dir given a directory to change
 * 
 * @retval 0: change succeed
 * @retval -1: change failed, no such directory
 * @retval -2: change failed, the given path is a file
 * @retval -3: change failed, the directory tree is too long
 */
int
dir_change(const char *dir) {
    pcb_t *cur_pcb = get_current_pcb();

    // get the absolute path
    char *abs = dyn_alloc(PGSIZE);
    bzero(abs, PGSIZE);

    // some special cases
    if (dir == 0) {
        // change to root directory when the given dir is null
        abs[0] = DIRNAME_ROOT_ASCII;
    } else if (strcmp(dir, DIR_CUR) == true) {
        // cd .
        dyn_free(abs);
        return 0;
    } else if (dir[0] == DIRNAME_ROOT_ASCII) {
        // cd <an absolute dir>
        memmove(abs, dir, strlen(dir));
        uint32_t len = strlen(abs);
        if (abs[len - 1] != DIR_SEPARATOR)    abs[len] = DIR_SEPARATOR;
    } else {
        // cd ..
        // cd <a relative dir>
        if (curdir_get(cur_pcb->curdir_, abs, PGSIZE) == -1) {
            dyn_free(abs);
            return -3;
        }

        if (strcmp(dir, DIR_PRE) == false) {
            memmove(abs + strlen(abs), dir, strlen(dir));
            uint32_t len = strlen(abs);
            if (abs[len - 1] != DIR_SEPARATOR)    abs[len] = DIR_SEPARATOR;
        } else {
            get_parent_child_filename(abs, 0);
        }
    }

    // search the corresponding directory item
    diritem_t *cur_diritem = dyn_alloc(sizeof(diritem_t));
    bzero(cur_diritem, sizeof(diritem_t));
    if (diritem_find(abs, cur_diritem) == false) {
        dyn_free(cur_diritem);
        dyn_free(abs);
        return -1;
    }

    if (cur_diritem->type_ == INODE_TYPE_FILE) {
        dyn_free(cur_diritem);
        dyn_free(abs);
        return -2;
    }

    dyn_free(cur_diritem);
    curdir_set(cur_pcb->curdir_, abs);
    // also influence the parent directory
    curdir_set(thread_curdir_get(cur_pcb->parent_), abs);
    dyn_free(abs);
    return 0;
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
    return curdir_get(get_current_pcb()->curdir_, buff, bufflen);
}
