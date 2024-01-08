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
 * @param inode inode corresponding to this dir
 * @param name  name corresponding to this dir
 */
void
set_dir_item(dir_item_t *dir, enum_inode_type type, inode_t *inode,
const char *name) {

    if (name == null)    panic("set_dir_item");

    dir->type_  = type;
    dir->inode_ = inode;
    memmove(dir->name_, name, strlen(name));
}

/**
 * @brief Set up the root dir
 */
void
create_root_dir() {

    // 1. setup dir item
    set_dir_item(&__fs_root_dir, INODE_TYPE_DIR,
        &__fs_inodes[INODE_INDEX_ROOT], "/");

    // 2. setup inode
#define FUNC_GET_DIR_SIZE(item_amount) \
    ((item_amount) * sizeof(dir_item_t))

    bzero(&__fs_inodes[INODE_INDEX_ROOT], sizeof(inode_t));
    uint32_t free_block = allocate_free_block();
    set_inode(&__fs_inodes[INODE_INDEX_ROOT],
        FUNC_GET_DIR_SIZE(strlen(__fs_dir_basic[FS_DIR_BASIC_PRE])),
        free_block);
    inode_to_disk(INODE_INDEX_ROOT);

    // 3. setup block
    dir_item_t dir_cur, dir_pre;
    set_dir_item(&dir_cur, INODE_TYPE_DIR,
        &__fs_inodes[INODE_INDEX_ROOT], __fs_dir_basic[FS_DIR_BASIC_CUR]);
    set_dir_item(&dir_pre, INODE_TYPE_DIR, null,
        __fs_dir_basic[FS_DIR_BASIC_PRE]);

    uint8_t sec[BYTES_SECTOR];
    bzero(sec, sizeof(sec));
    memmove(sec, &dir_cur, sizeof(dir_item_t));
    memmove(sec + sizeof(dir_item_t), &dir_pre, sizeof(dir_item_t));
    atabuff_t atabuff;
    atabuff_set(&atabuff, sec, sizeof(sec), free_block, ATA_CMD_IO_WRITE);
    ata_driver_rw(&atabuff);
}
