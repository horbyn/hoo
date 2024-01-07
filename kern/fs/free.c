/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "free.h"

static uint8_t __fs_map_free[BYTES_SECTOR];
static free_layout_t __fs_free_layout[MAX_FS_ACCESS_LEVEL];

void
init_free_layout() {
    bzero(&__fs_free_layout, sizeof(__fs_free_layout));

    const uint32_t MAX_DIRECT = 6,
        ITEMS_PER_SECTOR = BYTES_SECTOR / sizeof(lba_index_t);

#define FUNC_GET_FREE_BITMAP(free) \
    (((free) / BITS_PER_BYTE / BYTES_SECTOR) + 1)

    // For direct, all 8 bytes of inode block are direct.
    //   So a file occupies `MAX_INODE_BLOCKS(8)` sectors.
    //   Its bitmap occupies "`FREE_BITMAP_SEC_0`" sectors
    __fs_free_layout[0].level_ = INDEX_LEVEL0;
    __fs_free_layout[0].free_ = MAX_INODES * MAX_INODE_BLOCKS;
    __fs_free_layout[0].bitmap_free_ =
        FUNC_GET_FREE_BITMAP(__fs_free_layout[0].free_);

    // For single indirect, the previous 6 bytes are direct
    //   and the later 2 bytes single indirect. So a file occupies
    //   "`MAX_DIRECT`(6) + `ITEMS_PER_SECTOR`(128) * 2" sectors.
    //   Its bitmap as same as above
    __fs_free_layout[1].level_ = INDEX_LEVEL1;
    __fs_free_layout[1].free_ = MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR * 2);
    __fs_free_layout[1].bitmap_free_ =
        FUNC_GET_FREE_BITMAP(__fs_free_layout[1].free_);

    // For double indirect, the previous 6 bytes are direct,
    //   the following one is single indirect, the last one is
    //   double indirect. So a file occupies "`MAX_DIRECT`(6) 
    //   + `ITEMS_PER_SECTOR`(128) + `ITEMS_PER_SECTOR`^2 (128^2)"
    //   sectors. Its bitmap as same as above
    __fs_free_layout[2].level_ = INDEX_LEVEL2;
    __fs_free_layout[2].free_ =
        MAX_INODES * (MAX_DIRECT + ITEMS_PER_SECTOR
        + ITEMS_PER_SECTOR * ITEMS_PER_SECTOR);
    __fs_free_layout[2].bitmap_free_ =
        FUNC_GET_FREE_BITMAP(__fs_free_layout[2].free_);
}

const free_layout_t *
get_free_layout(uint32_t dev_sec) {

#define FUNC_GET_TOTAL_SECTORS(i)   (FS_LAYOUT_BASE_MAP_FREE + \
    (__fs_free_layout[i].bitmap_free_) + (__fs_free_layout[i].free_))

    if (dev_sec < FUNC_GET_TOTAL_SECTORS(0))
        panic("setup_inode_level()");
    else if (FUNC_GET_TOTAL_SECTORS(0) <= dev_sec
        && dev_sec < FUNC_GET_TOTAL_SECTORS(1))
        return &__fs_free_layout[0];
    else if (FUNC_GET_TOTAL_SECTORS(1) <= dev_sec
        && dev_sec < FUNC_GET_TOTAL_SECTORS(2))
        return &__fs_free_layout[1];
    else    return &__fs_free_layout[2];

    panic("get_free_layout()");
    return null;
}

uint32_t
free_block_allocate(const free_layout_t *free_layout) {
    uint32_t bit = 0;

    for (; bit < BYTES_SECTOR; ++bit) {
        if (bitmap_test(__fs_map_free, bit) == false) {
            break;
        }
    }

    if (bit == BYTES_SECTOR)
        panic("free_block_allocate(): no more free blocks");
    return (FS_LAYOUT_BASE_MAP_FREE + free_layout->bitmap_free_ + bit);
}

/**
 * @brief Set up the free blocks map
 *
 * @param is_new   a new disk
 */
void
setup_free_map(bool is_new) {
    ata_cmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bzero(__fs_map_free, sizeof(__fs_map_free));

    atabuff_t ata_buff;
    atabuff_set(&ata_buff, __fs_map_free, sizeof(__fs_map_free),
        FS_LAYOUT_BASE_MAP_FREE, cmd);
    ata_driver_rw(&ata_buff);
}
