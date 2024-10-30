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
 * @note
 * for "/usr/bin/" would get parent "/usr" and child "bin"
 * @note
 * for "/usr/bin" would also get parent "/usr" and child "bin"
 * @note
 * for "/" would get parent "" (null pointer) and child "/"
 * @param path   the specific filename
 * @param parent the parent name buffer (if exists)
 * @param child  the child name buffer (if exists)
 */
static void
get_parent_child_filename(const char *path, char *parent, char *child) {
    if (path == 0)    panic("get_parent_child_filename(): null pointer");

    int separator = -1;
    uint32_t path_sz = strlen(path);
    for (uint32_t i = path_sz - 1; i >= 0; --i) {
        if (path[i] == DIRNAME_ROOT_ASCII && i != path_sz - 1) {
            separator = i;
            break;
        }
    };

    if (separator == -1) {
        // corresponding to the case like "/"
        if (parent != 0)    parent[0] = 0;
        if (child != 0) {
            child[0] = DIRNAME_ROOT_ASCII;
            child[1] = 0;
        }
    } else {
        if (parent != 0) {
            if (separator > 0) {
                memmove(parent, path, separator);
                parent[separator] = 0;
            } else {
                parent[0] = DIRNAME_ROOT_ASCII;
                parent[1] = 0;
            }
        }
        if (child != 0) {
            memmove(child, path + separator + 1, path_sz - separator - 1);
            uint32_t child_sz = strlen(child);
            if (child[child_sz - 1] == DIRNAME_ROOT_ASCII)
                child[child_sz - 1] = 0;
            else    child[child_sz] = 0;
        }
    }

}

/**
 * @brief files system initialization
 */
void
filesystem_init(void) {
    // no need to release
    __fs_files = dyn_alloc(sizeof(files_t) * MAX_OPEN_FILES);
}

/**
 * @brief create a new file or directory (terminated with '/')
 * 
 * @param name the name to be create
 */
void
files_create(const char *name) {
    inode_type_t type = (name[strlen(name) - 1] == DIRNAME_ROOT_ASCII) ?
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
    char parent[DIRITEM_NAME_LEN], cur[DIRITEM_NAME_LEN];
    bzero(parent, sizeof(parent));
    bzero(cur, sizeof(cur));
    get_parent_child_filename(name, parent, cur);
    if (parent == 0)    panic("files_create(): parent directory is not found");
    if (!diritem_find(parent, di_parent))
        panic("files_create(): parent directory is not found");

    // create current diritem
    diritem_t *di_cur = diritem_create(type, cur, di_parent->inode_idx_);

    // current diritem pushes to parent's
    diritem_push(di_parent, di_cur);
    dyn_free(di_cur);

}

/**
 * @brief delete the specific file or directory
 * 
 * @param name filename or directory name
 * 
 * @retval 0: succeed
 * @retval -1: failed, parent directory is not found
 * @retval -2: failed, invalid format of parent directory
 * @retval -3: failed, file or directory is not found
 */
int
files_remove(const char *name) {
    if (name == 0)    panic("files_remove(): invalid filename");

    char parent[DIRITEM_NAME_LEN];
    get_parent_child_filename(name, parent, null);
    diritem_t di_parent, di_cur;
    if (!diritem_find(parent, &di_parent))    return -1;
    if (di_parent.inode_idx_ > MAX_INODES || di_parent.type_ != INODE_TYPE_DIR)
        return -2;
    if (!diritem_find(name, &di_cur))    return -3;

    diritem_remove(&di_parent, &di_cur);
    return 0;
}

/**
 * @brief open the specific file
 * 
 * @param name file name
 * @return file descriptor, -1 if the file is not found
 */
fd_t
files_open(const char *name) {
    if (name == 0)    panic("files_open(): null pointer");

    diritem_t *self = dyn_alloc(sizeof(diritem_t));
    if (!diritem_find(name, self))    return -1;
    if (self->type_ != INODE_TYPE_FILE || self->inode_idx_ > MAX_INODES)
        panic("files_open(): invalid file format");

    global_fd_t index = fd_global_alloc();
    __fs_files[index].inode_idx_ = self->inode_idx_;
    ++__fs_files[index].ref_;

    pcb_t *cur_pcb = get_current_pcb();
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
    if (buf == 0)    panic("files_read(): null pointer");

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_close(): the file was in non-opening");

    int inode_idx = __fs_files[index].inode_idx_;
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
    if (buf == 0 || size == 0)    return;

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_write(): the file was in non-opening");

    int inode_idx = __fs_files[index].inode_idx_;

    char *buf_tmp = dyn_alloc(BYTES_SECTOR);
    for (uint32_t i = 0; i <= size / BYTES_SECTOR; ++i) {
        if (i == size / BYTES_SECTOR && size % BYTES_SECTOR != 0) {
            // the last one needs to be filled with 0
            memmove(buf_tmp, buf + i * BYTES_SECTOR, size % BYTES_SECTOR);
            memset(buf_tmp + size % BYTES_SECTOR, 0,
                BYTES_SECTOR - size % BYTES_SECTOR);
        } else    memmove(buf_tmp, buf + i * BYTES_SECTOR, BYTES_SECTOR);

        uint32_t lba = iblock_get(inode_idx, i);
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
    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_get_size(): the file was in non-opening");

    return (__fs_inodes + __fs_files[index].inode_idx_)->size_;
}
