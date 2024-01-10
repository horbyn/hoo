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
 * @param inode_idx inode index
 * @param name  name corresponding to this dir
 */
void
set_dir_item(dir_item_t *dir, enum_inode_type type, idx_t inode_idx,
const char *name) {

    if (name == null)    panic("set_dir_item(): invalid dir name");
    if (inode_idx == MAX_INODES)
        panic("set_dir_item(): maximum index exceeded");

    dir->type_      = type;
    dir->inode_idx_ = inode_idx;
}

/**
 * @brief Set up the root dir
 * 
 * @param is_new new disk
 */
void
setup_root_dir(bool is_new) {

    // setup dir item
    set_dir_item(&__fs_root_dir, INODE_TYPE_DIR,
        INODE_INDEX_ROOT, "/");

    if (is_new) {

        uint8_t sec[BYTES_SECTOR];
        bzero(sec, sizeof(sec));

#define FUNC_GET_DIR_SIZE(item_amount) \
        ((item_amount) * sizeof(dir_item_t))

        // setup inode
        bzero(&__fs_inodes[INODE_INDEX_ROOT], sizeof(inode_t));
        uint32_t free_block = free_block_allocate();
        set_inode(&__fs_inodes[INODE_INDEX_ROOT],
            FUNC_GET_DIR_SIZE(strlen(__fs_dir_basic[FS_DIR_BASIC_PRE])),
            free_block);
        inode_rw_disk(INODE_INDEX_ROOT, ATA_CMD_IO_WRITE);

        // setup block
        dir_item_t dir_cur, dir_pre;
        set_dir_item(&dir_cur, INODE_TYPE_DIR,
            INODE_INDEX_ROOT, __fs_dir_basic[FS_DIR_BASIC_CUR]);
        set_dir_item(&dir_pre, INODE_TYPE_DIR, null,
            __fs_dir_basic[FS_DIR_BASIC_PRE]);

        memmove(sec, &dir_cur, sizeof(dir_item_t));
        memmove(sec + sizeof(dir_item_t), &dir_pre, sizeof(dir_item_t));
        ata_driver_rw(sec, sizeof(sec), free_block, ATA_CMD_IO_WRITE);
    }
}
