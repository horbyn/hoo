#include "files.h"
#include "dir.h"
#include "free.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"
#include "kern/driver/8042/8042.h"
#include "kern/driver/ata/ata_cmd.h"
#include "kern/driver/ata/ata.h"
#include "kern/driver/cga/cga.h"
#include "kern/module/io.h"
#include "user/lib.h"

files_t *__fs_files;
typedef fd_t global_fd_t;

/**
 * @brief 全局文件描述符分配
 * 
 * @return 全局文件描述符
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
 * @brief 全局文件描述符释放
 * 
 * @param fd 全局文件描述符
 */
static void
fd_global_free(global_fd_t fd) {
    if (fd >= MAX_OPEN_FILES)    panic("fd_global_free(): invalid file descriptor");
    __fs_files[fd].ref_ = 0;
}

/**
 * @brief 文件系统初始化
 */
void
filesystem_init(void) {
    // 不需要释放
    __fs_files = dyn_alloc(sizeof(files_t) * MAX_OPEN_FILES);
}

/**
 * @brief 创建一个新文件或目录（目录以 '/' 结尾）
 * 
 * @param name 文件或目录名
 * 
 * @retval 0: 创建成功
 * @retval -1: 创建失败, 文件或目录已经存在
 */
int
files_create(const char *name) {
    inode_type_t type = (name[strlen(name) - 1] == DIRNAME_ROOT_ASCII) ?
        INODE_TYPE_DIR : INODE_TYPE_FILE;

    // 不要重复创建
    diritem_t *self = dyn_alloc(sizeof(diritem_t));
    if (diritem_find(name, self)) {
        dyn_free(self);
        return -1;
    }
    dyn_free(self);

    // 获取父目录的 inode
    diritem_t *di_parent = dyn_alloc(sizeof(diritem_t));
    char parent[DIRITEM_NAME_LEN], cur[DIRITEM_NAME_LEN];
    bzero(parent, sizeof(parent));
    memmove(parent, name, strlen(name));
    bzero(cur, sizeof(cur));
    get_parent_child_filename(parent, cur);
    if (!diritem_find(parent, di_parent))
        panic("files_create(): parent directory is not found");

    // 为指定文件或目录创建目录项
    diritem_t *di_cur = diritem_create(type, cur, di_parent->inode_idx_);

    // 上一步目录项写入父目录项
    diritem_push(di_parent, di_cur);
    dyn_free(di_parent);
    dyn_free(di_cur);

    return 0;
}

/**
 * @brief 删除文件或目录
 * 
 * @param name 指定一个名称
 * 
 * @retval 0: 删除成功
 * @retval -1: 删除失败, 找不到父目录
 * @retval -2: 删除失败, 父目录项类型无效
 * @retval -3: 删除失败, 文件或目录不存在
 */
int
files_remove(const char *name) {
    if (name == 0)    panic("files_remove(): invalid filename");
    if (is_root_dir(name))    return -2;

    char parent[DIRITEM_NAME_LEN];
    bzero(parent, sizeof(parent));
    memmove(parent, name, strlen(name));
    get_parent_child_filename(parent, 0);
    diritem_t di_parent, di_cur;
    if (!diritem_find(parent, &di_parent))    return -1;
    if (di_parent.inode_idx_ > MAX_INODES || di_parent.type_ != INODE_TYPE_DIR)
        return -2;
    if (!diritem_find(name, &di_cur))    return -3;

    diritem_remove(&di_parent, &di_cur);
    return 0;
}

/**
 * @brief 打开指定文件
 * 
 * @param name 文件名
 * @return 文件描述符或 -1，表示打开失败
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
 * @brief 关闭文件描述符
 * 
 * @param fd 文件描述符
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
 * @brief 从指定文件中读取数据
 * 
 * @param fd   文件描述符
 * @param buf  数据缓冲区
 * @param size 缓冲区大小
 */
void
files_read(fd_t fd, void *buf, uint32_t size) {
    if (fd > MAX_FILES_PER_TASK)    panic("files_read(): invalid fd");
    if (buf == null)    panic("files_read(): null pointer");

    if (fd == FD_STDIN) {
        cclbuff_t *cclbuff = get_kb_buff();
        for (uint32_t i = 0; i < size; ++i) {
            *((char *)buf + i) = cclbuff_get(cclbuff);
        }
        return;
    }

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
 * @brief 向指定文件写入数据
 * 
 * @param fd   文件描述符
 * @param buf  数据缓冲区
 * @param size 缓冲区大小
 */
void
files_write(fd_t fd, const char *buf, uint32_t size) {
    if (fd > MAX_FILES_PER_TASK)    panic("files_write(): invalid fd");
    if (buf == 0 || size == 0)    return;
    if (fd == FD_STDIN)    panic("files_write(): not allowd to write to stdin");

    if (fd == FD_STDOUT || fd == FD_STDERR) {
        cga_putstr(buf, size);
        return;
    }

    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_write(): the file was in non-opening");

    int inode_idx = __fs_files[index].inode_idx_;

    char *buf_tmp = dyn_alloc(BYTES_SECTOR);
    for (uint32_t i = 0; i <= size / BYTES_SECTOR; ++i) {
        if (i == size / BYTES_SECTOR && size % BYTES_SECTOR != 0) {
            // 最后一个扇区不足 512B 部分填充 0
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

    // 更新 inode 和空闲块
    __fs_inodes[inode_idx].size_ = size;
    inodes_rw_disk(inode_idx, ATA_CMD_IO_WRITE);
    free_map_update();

    dyn_free(buf_tmp);
}

/**
 * @brief 获取指定文件的文件大小
 * 
 * @param fd 文件描述符
 * @return 文件大小
 */
uint32_t
files_get_size(fd_t fd) {
    pcb_t *cur_pcb = get_current_pcb();
    global_fd_t index = fmngr_files_get(cur_pcb->fmngr_, fd);
    if (__fs_files[index].ref_ == 0)
        panic("files_get_size(): the file was in non-opening");

    return (__fs_inodes + __fs_files[index].inode_idx_)->size_;
}

/**
 * @brief 输出当前目录下的所有文件
 * 
 * @param dir_or_file 指定一个目录或文件名
 * @retval 0: 无异常
 * @retval -1: 没有这个目录或文件
 */
int
files_list(const char *dir_or_file) {
    char *absolute = dyn_alloc(PGSIZE);
    bzero(absolute, PGSIZE);

    if (dir_or_file == 0 || (dir_or_file != 0 && dir_or_file[0] != DIRNAME_ROOT_ASCII)) {
        if (curdir_get(get_current_pcb()->curdir_, absolute, PGSIZE) == -1) {
            dyn_free(absolute);
            return -1;
        }

        if (dir_or_file != 0 && dir_or_file[0] != DIRNAME_ROOT_ASCII)
            memmove(absolute + strlen(absolute), dir_or_file, strlen(dir_or_file));
    } else    memmove(absolute, dir_or_file, strlen(dir_or_file));

    diritem_t *found = dyn_alloc(sizeof(diritem_t));
    if (diritem_find(absolute, found) == false)    return -1;

    if (found->type_ == INODE_TYPE_FILE) {
        kprintf("%dB\t\t%s\n", (__fs_inodes + found->inode_idx_)->size_, absolute);
    } else if (found->type_ == INODE_TYPE_DIR) {
        char *dir = diritem_traversal(found);
        for (uint32_t i = 0; i < __fs_inodes[found->inode_idx_].size_; ++i) {
            kprintf("%s\t", dir);
            dir += DIRITEM_NAME_LEN;
        }
        kprintf("\n");
        dyn_free(dir);
    } else    return -1;

    dyn_free(found);
    if (dir_or_file == null || absolute[0] != DIRNAME_ROOT_ASCII)
        dyn_free(absolute);
    return 0;
}
