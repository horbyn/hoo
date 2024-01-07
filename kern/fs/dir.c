/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "dir.h"

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
setup_root_dir() {
    bzero(&__fs_root_dir, sizeof(dir_item_t));

    inode_t *iroot = get_root_inode();
    bool is_root_exist = bitmap_test(iroot, INODE_INDEX_ROOT);
    if (is_root_exist)    return;

    // only `.` and `..` dirs the root had
    set_dir_item(&__fs_root_dir, INODE_TYPE_DIR,
        &iroot[INODE_INDEX_ROOT], ".");
    dir_item_t dir_pre;
    set_dir_item(&dir_pre, INODE_TYPE_DIR, null, "..");

    bzero(&iroot[INODE_INDEX_ROOT], sizeof(inode_t));
    iroot[INODE_INDEX_ROOT].size_ = 2;
}
