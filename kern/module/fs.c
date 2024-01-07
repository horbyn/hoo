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
    /****************************************************************************
     * DISK LAYOUT :                                                            *
     * ┌──────────┬─────────────┬────────────┬────────────┬──────────┬──────┐   *
     * │ RESERVED │ SUPER_BLOCK │ MAP_INODES │   INODES   │ MAP_FREE │ FREE │   *
     * └──────────┴─────────────┴────────────┴────────────┴──────────┴──────┘   *
     * |          \             \            \            \          \          *
     * | 1 sector  \  1 sector   \  1 sector  \ 64 sectors \ 1 sector \         *
     * |   LBA-0    \    LBA-1    \   LBA-2    \ LBA-3..66  \ LBA-67.. \ LBA-.. *
     *                                                                          *
     * NOTE : the first sector will, if necessary, use for Partition            *
     ****************************************************************************/

    uint32_t dev_sectors =
        ata_space.device_info_[ata_space.current_select_].total_sectors_;

    super_block_t super_block;
    bzero(&super_block, sizeof(super_block_t));

    init_free_layout();
    const free_layout_t *free_layout = get_free_layout(dev_sectors);
    uint32_t free_lba = FS_LAYOUT_BASE_MAP_FREE + free_layout->bitmap_free_;
    bool is_new = setup_super_block(&super_block, free_lba, free_layout->level_);

    setup_inode(is_new);
    setup_free_map(is_new);
    setup_root_dir();
}
