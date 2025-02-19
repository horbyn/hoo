#include "dir.h"
#include "free.h"
#include "kern/panic.h"
#include "kern/dyn/dynamic.h"
#include "kern/driver/ata/ata_cmd.h"
#include "kern/hoo/thread_curdir.h"
#include "user/lib.h"

/**
 * @brief 获取一个新的目录块
 * 
 * @param result 新目录块缓冲区
 * @param self   新目录块的 inode 索引
 * @param parent 新目录块的父目录 inode 索引
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
 * @brief 在目录块里面设置目录项
 * 
 * @param db          指定一个目录块
 * @param block_index 目录块数组下标
 * @param item        要设置的目录项
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
 * @brief 从目录块中获取目录项
 * 
 * @param db          指定一个目录块
 * @param block_index 目录块数组下标
 * @param result      保存结果的缓冲区
 */
static void
dirblock_get(const dirblock_t *db, uint32_t block_index, diritem_t *result) {
    if (db == null || result == null)    panic("dirblock_get(): null pointer");
    memmove(result, db->dir_ + block_index, sizeof(diritem_t));
}

/**
 * @brief 查找一个指定的目录项
 * 
 * @param di     目录项对象
 * @param search 要查找的目录名的绝对路径
 * @retval -1: 找不到
 * @retval 其他值: 目录项所在的目录块数组的下标
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
 * @brief 递归移除目录项（仅涉及索引，不涉及磁盘读写）
 * 
 * @param di 要移除的目录项
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
            // 跳过 . 和 ..
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
 * @brief 填充目录项结构
 * 
 * @param dir       要填充的目录项
 * @param type      inode 类型
 * @param inode_idx inode 索引
 * @param name      目录项对应目录名
 */
void
diritem_set(diritem_t *dir, inode_type_t type, int inode_idx, const char *name) {
    if (dir == null || name == 0)    panic("diritem_set(): null pointer");

    dir->type_      = type;
    dir->inode_idx_ = inode_idx;
    memmove(dir->name_, name, strlen(name) + 1);
}

/**
 * @brief 判断目录名是否根目录
 * 
 * @param dir 指定一个目录名
 * @retval true: 根目录
 * @retval false: 不是根目录
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
 * @brief 查找给定的目录项
 * 
 * @param dir   要查找的目录名的绝对路径
 * @param found 保存结果的缓冲区
 * 
 * @retval true:  找到
 * @retval false: 没找到
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
 * @brief 遍历目录项
 * 
 * @param dir 目录项
 * @return 动态分配的目录项对象（由 caller 释放）
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
 * @brief 为当前目录创建目录项
 * 
 * @param type         文件类型
 * @param item_name    目录项的目录名
 * @param parent_inode 父目录的 inode 索引
 * 
 * @return 动态分配的目录项对象（由 caller 释放）
 */
diritem_t *
diritem_create(inode_type_t type, const char *item_name, int parent_inode) {
    diritem_t *di = dyn_alloc(sizeof(diritem_t));
    bzero(di, sizeof(diritem_t));
    int inode_cur = inode_allocate();
    inode_map_setup(inode_cur, true);
    diritem_set(di, type, inode_cur, item_name);

    // 用来初始化 inode
    int free = free_allocate();
    free_map_setup(free, true);

    if (type == INODE_TYPE_FILE) {
        inode_set(inode_cur, 0, free);
    } else if (type == INODE_TYPE_DIR) {
        inode_set(inode_cur, 2, free);

        // 写入磁盘的目录块
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
 * @brief 将当前目录的 diritem 写入父目录的 diritem
 * 
 * @param parent 父目录的 diritem
 * @param cur    当前目录的 diritem
 */
void
diritem_push(diritem_t *parent, diritem_t *cur) {
    if (parent == null || cur == null)    panic("diritem_push(): null pointer");

    // 获取父目录的目录块
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

    // 写入父目录
    uint32_t db_index =
        __fs_inodes[parent->inode_idx_].size_ % MAX_DIRITEM_PER_BLOCK;
    memmove(&db->dir_[db_index], cur, sizeof(diritem_t));
    ++__fs_inodes[parent->inode_idx_].size_;
    free_rw_disk(db, block_lba, ATA_CMD_IO_WRITE);
    inodes_rw_disk(parent->inode_idx_, ATA_CMD_IO_WRITE);

    dyn_free(db);
}

/**
 * @brief 将当前目录的 diritem 从父目录的 diritem 中移除
 * 
 * @param parent 父目录的 diritem
 * @param cur    当前目录的 diritem
 * 
 * @retval 0: 成功
 * @retval -1: 失败，没有这个目录
 */
int
diritem_remove(diritem_t *parent, diritem_t *cur) {
    if (parent == null || cur == null)    panic("diritem_remove(): null pointer");

    int result = diritem_find_sub(parent, cur->name_);
    if (result == -1)    return -1;

    uint32_t lba = iblock_get(parent->inode_idx_, result / MAX_DIRITEM_PER_BLOCK);
    if (lba < __super_block.lba_free_)    panic("diritem_remove(): bug");

    // 从父目录中移除
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    free_rw_disk(db, lba, ATA_CMD_IO_READ);
    dirblock_set(db, result % MAX_DIRITEM_PER_BLOCK, null);
    free_rw_disk(db, lba, ATA_CMD_IO_WRITE);
    --__fs_inodes[parent->inode_idx_].size_;
    inodes_rw_disk(parent->inode_idx_, ATA_CMD_IO_WRITE);

    // 移除自己里面的目录和文件
    diritem_remove_sub(cur);
    free_map_update();
    inode_map_update();

    dyn_free(db);
    return 0;
}

/**
 * @brief 获取根目录的目录项
 * 
 * @return 根目录 diritem
 */
diritem_t **
get_root_dir(void) {
    static diritem_t *__fs_root_dir;
    return &__fs_root_dir;
}

/**
 * @brief 切换当前目录
 * 
 * @param dir 要切换的目录名
 * 
 * @retval 0: 切换成功
 * @retval -1: 切换失败, 没有这个目录
 * @retval -2: 切换失败, 给定名称是一个文件
 * @retval -3: 切换失败, 目录名太长
 */
int
dir_change(const char *dir) {
    pcb_t *cur_pcb = get_current_pcb();

    // 获取绝对路径
    char *abs = dyn_alloc(PGSIZE);
    bzero(abs, PGSIZE);

    // 一些特例
    if (dir == 0) {
        // 空的形参表示切换至根目录
        abs[0] = DIRNAME_ROOT_ASCII;
    } else if (strcmp(dir, DIR_CUR) == true) {
        // cd .
        dyn_free(abs);
        return 0;
    } else if (dir[0] == DIRNAME_ROOT_ASCII) {
        // cd <绝对路径>
        memmove(abs, dir, strlen(dir));
        uint32_t len = strlen(abs);
        if (abs[len - 1] != DIR_SEPARATOR)    abs[len] = DIR_SEPARATOR;
    } else {
        // cd ..
        // cd <相对路径>
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

    // 查找对应的目录项
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
    // 必须同时影响父目录的 curdir
    curdir_set(thread_curdir_get(cur_pcb->parent_), abs);
    dyn_free(abs);
    return 0;
}

/**
 * @brief 获取 curdir 对象
 * 
 * @param buff    保存结构的缓冲区
 * @param bufflen 缓冲区大小
 * 
 * @retval 0: 成功
 * @retval -1: 失败，并且缓冲区会填充空字符
 */
int
dir_get_current(char *buff, uint32_t bufflen) {
    return curdir_get(get_current_pcb()->curdir_, buff, bufflen);
}
