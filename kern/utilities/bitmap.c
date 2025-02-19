#include "bitmap.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief 位图结构初始化
 * 
 * @param map        位图
 * @param len_inbits 位图长度，以比特为单位
 * @param buff       位图缓冲区，以字节为单位
 */
void
bitmap_init(bitmap_t *map, uint32_t len_inbits, void *buff) {
    if (map == null)    panic("bitmap_init(): null pointer");
    bzero(buff, len_inbits / BITS_PER_BYTE);
    map->len_inbits_ = len_inbits;
    if (buff == null)    map->buff_ = null;
    else    map->buff_ = (uint8_t *)buff;
    map->prev_free_ = 0;
}

/**
 * @brief 测试位图中指定位置的比特
 * 
 * @param map 位图
 * @param idx 位图数组索引
 * @retval true:  置位
 * @retval false: 清位
 */
bool
bitmap_test(bitmap_t *map, int idx) {
    if (map == null)    panic("bitmap_test(): null pointer");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_test(): invalid index");

    int byte = idx / BITS_PER_BYTE;
    int bit = idx % BITS_PER_BYTE;

    return (map->buff_[byte] & ((uint8_t)true << bit));
}

/**
 * @brief 设置位图中指定比特位
 * 
 * @param map 位图
 * @param idx 位图数组索引
 */
void
bitmap_set(bitmap_t *map, int idx) {
    if (map == null)    panic("bitmap_set(): null pointer");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_set(): invalid index");

    int byte = idx / BITS_PER_BYTE;
    int bit = idx % BITS_PER_BYTE;

    map->buff_[byte] |= ((uint8_t)true << bit);
}

/**
 * @brief 清位位图中的指定比特位
 * 
 * @param map 位图
 * @param idx 位图数组索引
 */
void
bitmap_clear(bitmap_t *map, int idx) {
    if (map == null)    panic("bitmap_clear(): null pointer");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_clear(): invalid index");

    int byte = idx / BITS_PER_BYTE;
    int bit = idx % BITS_PER_BYTE;

    map->buff_[byte] &= ~((uint8_t)true << bit);
}

/**
 * @brief 查找位图以寻找一个特定的比特位（不会改变位图）
 * 
 * @param map 位图
 * @return 第一个特定的比特位
 */
int
bitmap_scan_empty(bitmap_t *map) {
    if (map == null)    panic("bitmap_scan_empty(): null pointer");
    if (map->prev_free_ == INVALID_INDEX || map->prev_free_ >= map->len_inbits_)
        panic("bitmap_scan_empty(): invalid parameters");

    uint32_t bit = map->prev_free_, i = 0;
    for (; i < map->len_inbits_; ++i) {
        bool is_set = bitmap_test(map, bit);
        if (is_set == false) {
            map->prev_free_ = (bit + 1) % map->len_inbits_;
            break;
        } else    bit = (bit + 1) % map->len_inbits_;
    }

    if (i == map->len_inbits_)    panic("bitmap_scan_empty(): no more free bits");
    return bit;
}
