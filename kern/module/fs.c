#include "fs.h"
#include "kern/dyn/dynamic.h"
#include "kern/fs/dir.h"
#include "kern/fs/free.h"
#include "kern/driver/ata/ata.h"

/**
 * @brief 初始化磁盘布局
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

    // 现在是关中断的
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

    // 在开中断之后生效
    ata_driver_change_mode(ATA_METHOD_IRQ);

}
