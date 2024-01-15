/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "fs.h"

/**
 * @brief initialize the disk layout
 */
void
init_fs(void) {
    /********************************************************************************
     * DISK LAYOUT :                                                                *
     * ┌──────────┬─────────────┬────────────┬────────────┬────────────┬────────┐   *
     * │ RESERVED │ SUPER_BLOCK │ MAP_INODES │   INODES   │ MAP_BLOCKS │ BLOCKS │   *
     * └──────────┴─────────────┴────────────┴────────────┴────────────┴────────┘   *
     * |          \             \            \            \            \            *
     * | 1 sector  \  1 sector   \  1 sector  \ 64 sectors \ n sectors  \           *
     * |   LBA-0    \    LBA-1    \   LBA-2    \ LBA-3..66  \  LBA-67..  \ LBA-..   *
     *                                                                              *
     * NOTE : the first sector will, if necessary, use for Partition                *
     ********************************************************************************/

    bool is_new = setup_super_block();
    setup_inode(is_new);
    setup_blocks_map(is_new);
    setup_root_dir(is_new);

    kprintf("================ FILE  SYSTEM ================"
        "\nstate:           %s"
        "\naccess:          %s"
        "\nlba:"
        "\n    super block: %d"
        "\n    inode map:   %d"
        "\n    inodes:      %d"
        "\n    block map:   %d"
        "\n    blocks:      %d\n\n",
        (is_new ? "new" : "old"),
        MACRO_STRING_INDEX_LEVEL(__super_block.index_level_),
        __super_block.lba_super_block_, __super_block.lba_map_inode_,
        __super_block.lba_inodes_, __super_block.lba_map_blocks_,
        __super_block.lba_blocks_);
}
