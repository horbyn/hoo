/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "fs.h"

/**
 * @brief initialize the disk layout
 */
void
kinit_fs(void) {

    /********************************************************************************
     * DISK LAYOUT :                                                                *
     * ┌──────────┬─────────────┬────────────┬────────────┬────────────┬────────┐   *
     * │ RESERVED │ SUPER_BLOCK │ MAP_INODES │   INODES   │  MAP_FREE  │  FREE  │   *
     * └──────────┴─────────────┴────────────┴────────────┴────────────┴────────┘   *
     * |          \             \            \            \            \            *
     * | 1 sector  \  1 sector   \  1 sector  \ 64 sectors \ n sectors  \           *
     * |   LBA-0    \    LBA-1    \   LBA-2    \ LBA-3..66  \  LBA-67..  \ LBA-..   *
     *                                                                              *
     * NOTE : the first sector will, if necessary, use for Partition                *
     ********************************************************************************/

    // we are in interrupt disabling now
    ata_driver_change_mode(ATA_METHOD_POLLING);
    bool is_new = setup_super_block();
    setup_inode(is_new);
    setup_free_map(is_new);
    diritem_t **root = get_root_dir();
    if (is_new)
        (*root) = diritem_create(INODE_TYPE_DIR, DIRNAME_ROOT_STR, INVALID_INDEX);
    else {
        (*root) = dyn_alloc(sizeof(diritem_t));
        (*root)->inode_idx_ = 0;
        (*root)->type_ = INODE_TYPE_DIR;
        (*root)->name_[0] = DIRNAME_ROOT_ASCII;
        (*root)->name_[1] = 0;
    }
    filesystem_init();

    // take effect after interrupt enabling
    // ata_driver_change_mode(ATA_METHOD_IRQ);

}
