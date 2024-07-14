/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "files.h"

files_t *__fs_files;

/**
 * @brief files system initialization
 */
void
files_init(void) {
    // no need to release
    __fs_files = dyn_alloc(sizeof(files_t) * MAX_OPEN_FILES);
}

/**
 * @brief global file descriptor allocation
 * 
 * @return file descriptor
 */
static fd_t
fd_global_alloc(void) {
    fd_t i = 0;
    for (; i < MAX_OPEN_FILES; ++i) {
        if (__fs_files[i].used_ == false) {
            __fs_files[i].used_ = true;
            break;
        }
    }
    if (i == MAX_OPEN_FILES)
        panic("fd_global_alloc(): no free file descriptor");
    return i;
}

/**
 * @brief global file descriptor release
 * 
 * @param fd file descriptor
 */
static void
fd_global_free(fd_t fd) {
    if (fd >= MAX_OPEN_FILES)    panic("fd_global_free(): invalid file descriptor");
    __fs_files[fd].used_ = false;
}

/**
 * @brief get the parent filename
 * 
 * @param name   the specific filename
 * @param parent the parent name buffer
 */
static void
get_parent_filename(const char *name, char *parent) {
    if (name == null || parent == null)
        panic("get_parent_diritem(): null pointer");
    if (is_root_dir(name) == true)
        panic("files_create(): cannot create root directory");

    uint32_t sz = strlen(name);
    memmove(parent, name, sz);
    for (uint32_t i = sz - 1; i > 0; --i) {
        if (parent[i] == DIRNAME_ROOT_ASCII) {
            parent[i] = 0;
            break;
        } else    parent[i] = 0;
    }
}

/**
 * @brief remove the last backslash from filename (if present)
 * 
 * @param filename filename to be formatting
 * @param result   result buffer
 */
static void
filename_format(const char *filename, char *result) {
    if (filename == null || result == null)
        panic("filename_format(): null pointer");

    uint32_t sz = strlen(filename);
    memmove(result, filename, sz);
    if (result[sz - 1] == '/')    result[sz - 1] = 0;
}

/**
 * @brief create a new file or directory
 * 
 * @param name the name to be create
 */
void
files_create(const char *name) {
    char filename[DIRITEM_NAME_LEN];
    bzero(filename, sizeof(filename));
    filename_format(name, filename);
    enum_inode_type type = (name[strlen(name) - 1] == '/') ?
        INODE_TYPE_DIR : INODE_TYPE_FILE;

    // get parent inode
    diritem_t *di_parent = dyn_alloc(sizeof(diritem_t));
    char parent[DIRITEM_NAME_LEN];
    bzero(parent, sizeof(parent));
    get_parent_filename(filename, parent);
    diritem_find(parent, di_parent);

    // push to parent inode
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    uint32_t i = 0;
    lba_index_t lba = 0;
    idx_t inode_new = inode_allocate();
    for (; i < __super_block.inode_block_index_max_; ++i) {
        lba = iblock_get(di_parent->inode_idx_, i);
        if (lba == 0) {
            lba = free_allocate();
            __fs_inodes[di_parent->inode_idx_].iblocks_[i] = lba;
            ++__fs_inodes[di_parent->inode_idx_].size_;
            inodes_rw_disk(di_parent->inode_idx_, ATA_CMD_IO_WRITE);
            dirblock_get_new(db, inode_new, di_parent->inode_idx_);
            free_map_setup(lba, true);
            break;
        } else if (lba >= __super_block.lba_free_) {
            free_rw_disk(db, lba, ATA_CMD_IO_READ);
            if (db->amount_ < MAX_DIRITEM_PER_BLOCK)    break;
        } else    panic("files_create(): invalid iblock lba");
    } // end for()
    if (i == __super_block.inode_block_index_max_)
        panic("files_create(): files / dirs overflow!");

    // create new diritem
    diritem_t *di_new = dyn_alloc(sizeof(diritem_t));
    char name_new[DIRITEM_NAME_LEN];
    bzero(name_new, sizeof(name_new));
    strsep(filename, '/', -1, name_new);
    diritem_set(di_new, type, inode_new, name_new);
    diritem_write(db, di_new);
    free_rw_disk(db, lba, ATA_CMD_IO_WRITE);

    lba_index_t lba_new = free_allocate();
    free_map_setup(lba_new, true);
    if (type == INODE_TYPE_DIR) {
        // new diritem push to disk
        dirblock_t *db_new = dyn_alloc(sizeof(dirblock_t));
        dirblock_get_new(db_new, inode_new, di_parent->inode_idx_);
        inode_set(inode_new, 1, lba_new);
        free_rw_disk(db_new, lba_new, ATA_CMD_IO_WRITE);
        dyn_free(db_new);
    } else {
        inode_set(inode_new, 0, lba_new);
    }
    inodes_rw_disk(inode_new, ATA_CMD_IO_WRITE);
    inode_map_setup(inode_new, true);

    inode_map_update();
    free_map_update();

    dyn_free(db);
    dyn_free(di_new);
    dyn_free(di_parent);
}

/**
 * @brief delete directory item recursively (only for index, not involves disk rw)
 * 
 * @param di directory item
 */
static void
delete_diritem(diritem_t *di) {
    if (di == null)    return;

    if (di->type_ == INODE_TYPE_FILE) {
        for (uint32_t i = 0; i < __super_block.inode_block_index_max_; ++i) {
            lba_index_t lba = iblock_get(di->inode_idx_, i);
            if (lba != 0)    free_map_setup(lba, false);
            else    break;
        }

    } else {
        dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
        for (uint32_t i = 0; i < __fs_inodes[di->inode_idx_].size_; ++i) {
            lba_index_t dirblock_lba = iblock_get(di->inode_idx_, i);
            if (dirblock_lba == 0)    break;

            free_rw_disk(db, dirblock_lba, ATA_CMD_IO_READ);
            // skip . and ..
            for (uint32_t j = 0; j < db->amount_; ++j) {
                if ((i == 0 && j == 0 )|| (i == 0 && j == 1))    continue;
                delete_diritem(db->dir_ + j);
            } // end for(j)
            free_map_setup(dirblock_lba, false);
        } // end for(i)
        dyn_free(db);
    }

    inode_map_setup(di->inode_idx_, false);
}

/**
 * @brief delete the specific file or directory
 * 
 * @param name filename or directory name
 */
void
files_remove(const char *name) {
    if (name == null)    panic("files_remove(): invalid filename");
    char filename[DIRITEM_NAME_LEN];
    bzero(filename, sizeof(filename));
    filename_format(name, filename);

    // get parent inode
    diritem_t *di_parent = dyn_alloc(sizeof(diritem_t));
    char parent[DIRITEM_NAME_LEN];
    get_parent_filename(filename, parent);
    diritem_find(parent, di_parent);
    if (di_parent->inode_idx_ > MAX_INODES || di_parent->type_ != INODE_TYPE_DIR)
        panic("files_remove(): invalid format of parent");

    char cur_name[DIRITEM_NAME_LEN];
    bzero(cur_name, sizeof(cur_name));
    strsep(filename, '/', -1, cur_name);

    bool found = false;
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    for (uint32_t i = 0; i < __fs_inodes[di_parent->inode_idx_].size_; ++i) {
        lba_index_t dirblock_lba = iblock_get(di_parent->inode_idx_, i);
        if (dirblock_lba == 0)    break;

        free_rw_disk(db, dirblock_lba, ATA_CMD_IO_READ);
        for (uint32_t j = 0; j < db->amount_; ++j) {
            if ((i == 0 && j == 0 )|| (i == 0 && j == 1))    continue;

            if (memcmp(cur_name, db->dir_[j].name_, strlen(cur_name))) {
                delete_diritem(db->dir_ + j);
                free_map_update();
                inode_map_update();
                bzero(db->dir_ + j, sizeof(diritem_t));
                found = true;

                if (i != 0 && j == 1) {
                    // nothing if we deleted the last item in the dirblock
                    // so decrease the inode size
                    --__fs_inodes[di_parent->inode_idx_].size_;
                    inodes_rw_disk(di_parent->inode_idx_, ATA_CMD_IO_WRITE);
                }

                break;
            }
        } // end for(j)

        if (found) {
            --db->amount_;
            free_rw_disk(db, dirblock_lba, ATA_CMD_IO_WRITE);
            break;
        }
    } // end for(i)
    if (found == false)
        panic("files_remove(): file / directory not found");

    dyn_free(db);
    dyn_free(di_parent);
}

/**
 * @brief open the specific file
 * 
 * @param name file name
 * @return file descriptor
 */
fd_t
files_open(const char *name) {
    if (name == null)    panic("files_open(): invalid filename");

    diritem_t *self = dyn_alloc(sizeof(diritem_t));
    diritem_find(name, self);
    if (self->type_ != INODE_TYPE_FILE || self->inode_idx_ > MAX_INODES)
        panic("files_open(): invalid file format");

    fd_t fd = fd_global_alloc();
    __fs_files[fd].inode_idx_ = self->inode_idx_;

    dyn_free(self);
    return 0;
}

/**
 * @brief close the specific file
 * 
 * @param fd file descriptor
 */
void
files_close(fd_t fd) {
    fd_global_free(fd);
}
