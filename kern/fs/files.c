/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "files.h"

files_t *__fs_files;
typedef fd_t global_fd_t;

/**
 * @brief files system initialization
 */
void
filesystem_init(void) {
    // no need to release
    __fs_files = dyn_alloc(sizeof(files_t) * MAX_OPEN_FILES);
}

/**
 * @brief global file descriptor allocation
 * 
 * @return file descriptor
 */
static global_fd_t
fd_global_alloc(void) {
    global_fd_t i = 0;
    for (; i < MAX_OPEN_FILES; ++i) {
        if (__fs_files[i].ref_ == 0) {
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
 * @param fd global file descriptor
 */
static void
fd_global_free(global_fd_t fd) {
    if (fd >= MAX_OPEN_FILES)    panic("fd_global_free(): invalid file descriptor");
    __fs_files[fd].ref_ = 0;
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
 * @brief create a new file or directory (terminated with '/')
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

    // cannot create repeatly
    diritem_t *self = dyn_alloc(sizeof(diritem_t));
    if (diritem_find(name, self)) {
        dyn_free(self);
        return;
    }
    dyn_free(self);

    // get parent inode
    diritem_t *di_parent = dyn_alloc(sizeof(diritem_t));
    char parent[DIRITEM_NAME_LEN];
    bzero(parent, sizeof(parent));
    get_parent_filename(filename, parent);
    if (!diritem_find(parent, di_parent))
        panic("files_create(): parent directory is not found");

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
    if (!diritem_find(parent, di_parent))
        panic("files_remove(): parent directory is not found");
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

            if (strcmp(cur_name, db->dir_[j].name_)) {
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
 * @brief file manager initialization
 * 
 * @param fmngr file manager
 */
static fmngr_t *
fmngr_init(fmngr_t *fmngr) {
    if (fmngr != null)    return fmngr;

    // release when the task is terminated
    fmngr = dyn_alloc(sizeof(fmngr_t));
    fmngr->fd_set_ = dyn_alloc(sizeof(bitmap_t));
    fmngr->files_  = dyn_alloc(MAX_FILES_PER_TASK * sizeof(fd_t));
    bzero(fmngr->files_, MAX_FILES_PER_TASK * sizeof(fd_t));

    void *buff = dyn_alloc(MAX_FILES_PER_TASK / BITS_PER_BYTE);
    bitmap_init(fmngr->fd_set_, MAX_FILES_PER_TASK, buff);

    // for stdin, stdout, stderr
    bitmap_set(fmngr->fd_set_, FD_STDIN);
    bitmap_set(fmngr->fd_set_, FD_STDOUT);
    bitmap_set(fmngr->fd_set_, FD_STDERR);

    return fmngr;
}

/**
 * @brief open the specific file
 * 
 * @param name file name
 * @return file descriptor, -1 if the file is not found
 */
fd_t
files_open(const char *name) {
    if (name == null)    panic("files_open(): invalid filename");

    diritem_t *self = dyn_alloc(sizeof(diritem_t));
    if (!diritem_find(name, self))    return -1;
    if (self->type_ != INODE_TYPE_FILE || self->inode_idx_ > MAX_INODES)
        panic("files_open(): invalid file format");

    global_fd_t index = fd_global_alloc();
    __fs_files[index].inode_idx_ = self->inode_idx_;
    ++__fs_files[index].ref_;

    pcb_t *cur_pcb = get_current_pcb();
    cur_pcb->fmngr_ = fmngr_init(cur_pcb->fmngr_);
    fd_t fd = fmngr_alloc(cur_pcb->fmngr_);
    fmngr_files_set(cur_pcb->fmngr_, fd, index);

    dyn_free(self);
    return fd;
}

/**
 * @brief close the specific file
 * 
 * @param fd file descriptor
 */
void
files_close(fd_t fd) {
    if (fd > MAX_FILES_PER_TASK)    panic("files_close(): invalid fd");

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_close(): the file was already closed");

    if (__fs_files[index].ref_ > 0)    --__fs_files[index].ref_;
    if (__fs_files[index].ref_ == 0)    fd_global_free(index);
}

/**
 * @brief data reads from the specific file
 * 
 * @param fd   file descriptor
 * @param buf  buffer
 * @param size buffer size
 */
void
files_read(fd_t fd, char *buf, uint32_t size) {
    if (fd > MAX_FILES_PER_TASK)    panic("files_read(): invalid fd");
    if (buf == null)    panic("files_read(): invalid buffer");

    if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) {
        cclbuff_t *cclbuff = get_kb_buff();
        for (uint32_t i = 0; i < size; ++i) {
            buf[i] = cclbuff_get(cclbuff);
        }
        return;
    }

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_close(): the file was in non-opening");

    idx_t inode_idx = __fs_files[index].inode_idx_;
    inode_t *inode = __fs_inodes + inode_idx;
    if (inode->size_ == 0 || size == 0)    return;

    uint32_t total_size = (size > inode->size_) ? inode->size_ : size;
    uint32_t cr = (total_size + BYTES_SECTOR) / BYTES_SECTOR;
    char *temp_buf = dyn_alloc(BYTES_SECTOR);
    for (uint32_t i = 0; i < cr; ++i) {
        uint32_t cur_size = (i == cr - 1) ?
            (total_size - i * BYTES_SECTOR) : BYTES_SECTOR;

        free_rw_disk(temp_buf, iblock_get(inode_idx, i), ATA_CMD_IO_READ);
        memmove(buf + i * BYTES_SECTOR, temp_buf, cur_size);
    }
    dyn_free(temp_buf);
}

/**
 * @brief data writes to the specific file
 * 
 * @param fd   file descriptor
 * @param buf  buffer
 * @param size buffer size
 */
void
files_write(fd_t fd, const char *buf, uint32_t size) {
    if (fd > MAX_FILES_PER_TASK)    panic("files_write(): invalid fd");
    if (buf == null || size == 0)    return;

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_write(): the file was in non-opening");

    idx_t inode_idx = __fs_files[index].inode_idx_;

    char *buf_tmp = dyn_alloc(BYTES_SECTOR);
    for (uint32_t i = 0; i <= size / BYTES_SECTOR; ++i) {
        if (i == size / BYTES_SECTOR && size % BYTES_SECTOR != 0) {
            // the last one needs to be filled with 0
            memmove(buf_tmp, buf + i * BYTES_SECTOR, size % BYTES_SECTOR);
            memset(buf_tmp + size % BYTES_SECTOR, 0,
                BYTES_SECTOR - size % BYTES_SECTOR);
        } else    memmove(buf_tmp, buf + i * BYTES_SECTOR, BYTES_SECTOR);

        lba_index_t lba = iblock_get(inode_idx, i);
        if (lba == 0) {
            lba = free_allocate();
            free_map_setup(lba, true);
            iblock_set(inode_idx, i, lba);
        }
        free_rw_disk(buf_tmp, lba, ATA_CMD_IO_WRITE);
    } // end for(i)

    // update inode and free
    __fs_inodes[inode_idx].size_ = size;
    inodes_rw_disk(inode_idx, ATA_CMD_IO_WRITE);
    free_map_update();

    dyn_free(buf_tmp);
}

/**
 * @brief get the size of the specific file
 * 
 * @param fd file descriptor
 * @return the size of the file
 */
uint32_t
files_get_size(fd_t fd) {
    if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR)
        panic("files_get_size(): invalid file descriptor");

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_get_size(): the file was in non-opening");

    return (__fs_inodes + __fs_files[index].inode_idx_)->size_;
}
