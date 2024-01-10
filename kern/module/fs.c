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

    bool is_new = setup_super_block();
    setup_inode(is_new);
    setup_free_map(is_new);
    setup_root_dir(is_new);
}
