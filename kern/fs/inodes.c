#include "inodes.h"
#include "free.h"
#include "kern/panic.h"
#include "kern/driver/ata/ata.h"
#include "kern/utilities/bitmap.h"
#include "user/lib.h"

// inodes 位图 in-memory 结构（置位表示已经缓存至磁盘，清位表示已经缓存至内存）
static uint8_t __bmbuff_fs_inodes[BYTES_SECTOR];
// inodes in-memory 结构
inode_t __fs_inodes[MAX_INODES];
static bitmap_t __bmfs;

/**
 * @brief 初始化 inode 区域
 *
 * @param is_new 是否新的磁盘
 */
void
setup_inode(bool is_new) {

    atacmd_t cmd = is_new ? ATA_CMD_IO_WRITE : ATA_CMD_IO_READ;
    bitmap_init(&__bmfs, MAX_INODES, __bmbuff_fs_inodes);
    bzero(__fs_inodes, sizeof(__fs_inodes));
    bzero(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes));

    // inode 位图区域
    ata_driver_rw(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes),
        FS_LAYOUT_BASE_MAP_INODES, cmd);

    // 缓存所有的 inode
    if (is_new == false) {
        for (uint32_t i = 0; i < MAX_INODES; ++i) {
            // bitmap_test() will return non-zero
            if (bitmap_test(&__bmfs, i) != false)
                inodes_rw_disk(i, cmd);
        }
    }
}

/**
 * @brief 分配 inode
 * 
 * @return inode 数组下标
 */
int
inode_allocate() {
    return bitmap_scan_empty(&__bmfs);
}

/**
 * @brief 设置 inode 位图（只涉及 in-memory 结构，不涉及 on-disk）
 * 
 * @param inode_idx inode 索引
 * @param is_set    是否置位
 */
void
inode_map_setup(int inode_idx, bool is_set) {
    if (inode_idx == INVALID_INDEX)
        panic("inode_map_update(): invalid index");

    if (is_set)    bitmap_set(&__bmfs, inode_idx);
    else    bitmap_clear(&__bmfs, inode_idx);
}

/**
 * @brief 更新 inode 位图 on-disk 结构
 */
void
inode_map_update() {
    ata_driver_rw(__bmbuff_fs_inodes, sizeof(__bmbuff_fs_inodes),
        __super_block.lba_map_inode_, ATA_CMD_IO_WRITE);
}

/**
 * @brief 填充 inode 结构体
 * 
 * @param inode_idx inode 数组索引
 * @param size      inode 所表示文件的大小
 * @param base_lba  inode 所表示文件从哪个 LBA 开始
 */
void
inode_set(int inode_idx, uint32_t size, uint32_t base_lba) {
    if (inode_idx == INVALID_INDEX)
        panic("inode_set(): invalid inode");

    inode_t *inode = &__fs_inodes[inode_idx];
    bzero(inode, sizeof(inode_t));
    inode->size_ = size;
    inode->iblocks_[0] = base_lba;
}

/**
 * @brief inode 读写
 * 
 * @param inode_idx inode 数组索引
 * @param cmd       ATA 命令
 */
void
inodes_rw_disk(int inode_idx, atacmd_t cmd) {
    if (inode_idx == INVALID_INDEX)
        panic("inodes_write_to_disk(): invalid index");
    if (cmd == ATA_CMD_IO_READ && bitmap_test(&__bmfs, inode_idx) == false)
        panic("inodes_read_from_disk(): not allow to read from an empty inode");

    uint8_t sect[BYTES_SECTOR];
    bzero(sect, sizeof(sect));
    if (cmd == ATA_CMD_IO_WRITE)
        memmove(sect, __fs_inodes + inode_idx, sizeof(inode_t));
    ata_driver_rw(sect, sizeof(sect), __super_block.lba_inodes_ + inode_idx, cmd);
    if (cmd == ATA_CMD_IO_READ)
        memmove(__fs_inodes + inode_idx, sect, sizeof(inode_t));
}

/**
 * @brief inode 索引表处理函数，要么往里面写要么往里面读
 * 
 * @param inode_idx  inode 数组索引
 * @param iblock_idx inode 索引数组索引
 * @param write      要写入 inode 索引数组的元素
 * @param read       要从 inode 索引数组中读取出来的元素
 */
static void
iblock_handle(int inode_idx, int iblock_idx, uint32_t write, uint32_t *read) {
    if (inode_idx == INVALID_INDEX || inode_idx >= MAX_INODES)
        panic("iblock_handle(): invalid inode index");
    if (iblock_idx == INVALID_INDEX
        || iblock_idx >= __super_block.inode_block_index_max_)
        panic("iblock_handle(): invalid iblock index");
    if (write < __super_block.lba_free_ && read == null)
        panic("iblock_handle(): invalid lba");
    if (write >= __super_block.lba_free_ && read != null)
        panic("iblock_handle(): illegal operation");

    static const int DIRECT_INDEX = MAX_INODE_BLOCKS - 2 - 1,
        LEVEL1_INDEX = DIRECT_INDEX + 1, LEVEL2_INDEX = LEVEL1_INDEX + 1;
    static const int DIRECT_BORDER = MAX_INODE_BLOCKS - 2,
        LEVEL1_BORDER = DIRECT_BORDER + LBA_ITEMS_PER_SECTOR,
        LEVEL2_BORDER = LEVEL1_BORDER + LBA_ITEMS_PER_SECTOR * LBA_ITEMS_PER_SECTOR;

    inode_t *inode = __fs_inodes + inode_idx;
    char buf[BYTES_SECTOR];
    if (iblock_idx < DIRECT_BORDER) {
        if (read != null)    *read = inode->iblocks_[iblock_idx];
        else {
            inode->iblocks_[iblock_idx] = write;
            inodes_rw_disk(inode_idx, ATA_CMD_IO_WRITE);
        }
    } else if (iblock_idx < LEVEL1_BORDER) {
        uint32_t level1 = inode->iblocks_[LEVEL1_INDEX];
        if (level1 == 0) {
            if (read == null) {
                level1 = free_allocate();
                free_map_setup(level1, true);
                inode->iblocks_[LEVEL1_INDEX] = level1;
                inodes_rw_disk(inode_idx, ATA_CMD_IO_WRITE);
            }
            bzero(buf, BYTES_SECTOR);
        } else    ata_driver_rw(buf, BYTES_SECTOR, level1, ATA_CMD_IO_READ);

        if (read != null)    *read = ((uint32_t *)buf)[iblock_idx - DIRECT_BORDER];
        else {
            ((uint32_t *)buf)[iblock_idx - DIRECT_BORDER] = write;
            ata_driver_rw(buf, BYTES_SECTOR, level1, ATA_CMD_IO_WRITE);
        }
    } else if (iblock_idx < LEVEL2_BORDER) {
        uint32_t level2 = inode->iblocks_[LEVEL2_INDEX];
        if (level2 == 0) {
            if (read == null) {
                level2 = free_allocate();
                free_map_setup(level2, true);
                inode->iblocks_[LEVEL2_INDEX] = level2;
                inodes_rw_disk(inode_idx, ATA_CMD_IO_WRITE);
            }
            bzero(buf, BYTES_SECTOR);
        } else    ata_driver_rw(buf, BYTES_SECTOR, level2, ATA_CMD_IO_READ);

        int index = iblock_idx - LEVEL1_BORDER;
        uint32_t level1 = ((uint32_t *)buf)[index / LBA_ITEMS_PER_SECTOR];
        if (level1 == 0) {
            if (read == null) {
                level1 = free_allocate();
                free_map_setup(level1, true);
                ((uint32_t *)buf)[index / LBA_ITEMS_PER_SECTOR] = level1;
                ata_driver_rw(buf, BYTES_SECTOR, level2, ATA_CMD_IO_WRITE);
            }
            bzero(buf, BYTES_SECTOR);
        } else    ata_driver_rw(buf, BYTES_SECTOR, level1, ATA_CMD_IO_READ);

        if (read != null)    *read = ((uint32_t *)buf)[index % LBA_ITEMS_PER_SECTOR];
        else {
            ((uint32_t *)buf)[index % LBA_ITEMS_PER_SECTOR] = write;
            ata_driver_rw(buf, BYTES_SECTOR, level1, ATA_CMD_IO_WRITE);
        }
    } else {
        panic("iblock_handle(): invalid iblock index");
    }
}

/**
 * @brief 将指定 inode 中的索引数组元素设置为指定的 lba
 * 
 * @param inode_idx  inode 数组索引
 * @param iblock_idx inode 的索引数组索引
 * @param lba        LBA
 */
void
iblock_set(int inode_idx, int iblock_idx, uint32_t lba) {
    iblock_handle(inode_idx, iblock_idx, lba, null);
}

/**
 * @brief 从 inode 中的索引数组中读取 LBA
 * 
 * @param inode_idx  inode 数组索引
 * @param iblock_idx inode 的索引数组索引
 * @return LBA
 */
uint32_t
iblock_get(int inode_idx, int iblock_idx) {
    uint32_t ret = 0;
    iblock_handle(inode_idx, iblock_idx, 0, &ret);
    return ret;
}
