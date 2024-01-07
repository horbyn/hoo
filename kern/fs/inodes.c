/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "inodes.h"

static uint8_t __fs_map_inodes[BYTES_SECTOR];               // inodes map in-memory c
static inode_t __fs_inodes[MAX_INODES];                     // inodes in-memory cache

inode_t *
get_root_inode(void) {
    return &(__fs_inodes[0]);
}

inode_t *
inode_allocate()
{
    return null;
}

/**
 * @brief Set up inode metadata
 *
 * @param is_new a new disk
 */
void
setup_inode(bool is_new) {

    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_inodes, sizeof(__fs_map_inodes));
    bzero(__fs_inodes, sizeof(__fs_inodes));

    // inode map layout
    atabuff_t ata_buff;
    atabuff_set(&ata_buff, __fs_map_inodes, sizeof(__fs_map_inodes),
        FS_LAYOUT_BASE_MAP_INODES, cmd);
    ata_driver_rw(&ata_buff);

    // inodes layout
    for (size_t i = 0; i < MAX_INODES; ++i) {
        atabuff_set(&ata_buff, &__fs_inodes[i], sizeof(__fs_inodes[i]),
            FS_LAYOUT_BASE_INODES + i, cmd);
        ata_driver_rw(&ata_buff);
    }
}
