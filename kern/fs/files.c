/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "files.h"

files_t __fs_files[MAX_OPEN_FILES];

bool
files_create(enum_inode_type type, const char *name) {
    // get parent inode
    diritem_t *di_parent = dyn_alloc(sizeof(diritem_t));
    diritem_find(name, di_parent);

    // push to parent inode
    dirblock_t *db = dyn_alloc(sizeof(dirblock_t));
    uint32_t i = 0;
    lba_index_t lba = 0;
    bool is_new = false;
    for (; i < __super_block.inode_block_index_max_; ++i) {
        lba = iblock_get(di_parent->inode_idx_, i);
        if (lba == 0) {
            lba = free_allocate();
            __fs_inodes[di_parent->inode_idx_].iblocks_[i] = lba;
            dirblock_get_new(db);
            is_new = true;
            break;
        } else if (lba >= __super_block.lba_free_) {
            free_rw_disk(db, lba, ATA_CMD_IO_READ, false);
            if (db->amount_ < MAX_DIRITEM_PER_BLOCK)    break;
        } else    panic("files_create(): invalid iblock lba");
    } // end for()
    if (i == __super_block.inode_block_index_max_)
        panic("files_create(): files / dirs overflow!");

    // create new diritem
    diritem_t *di_new = dyn_alloc(sizeof(diritem_t));
    idx_t inode_new = inode_allocate();
    char name_new[DIRITEM_NAME_LEN];
    bzero(name_new, sizeof(name_new));
    strsep(name, '/', -1, name_new);
    diritem_set(di_new, type, inode_new, name_new);
    diritem_write(db, di_new);
    free_rw_disk(db, lba, ATA_CMD_IO_WRITE, is_new);
    ++__fs_inodes[di_parent->inode_idx_].size_;

    // new diritem push to disk
    lba_index_t lba_new = free_allocate();
    dirblock_t *db_new = dyn_alloc(sizeof(dirblock_t));
    dirblock_get_new(db_new);
    inode_set(inode_new, db_new->amount_, lba_new);
    free_rw_disk(db_new, lba_new, ATA_CMD_IO_WRITE, true);

    dyn_free(db_new);
    dyn_free(db);
    dyn_free(di_new);
    dyn_free(di_parent);
    return true;
}
