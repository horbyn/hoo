/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "bitmap.h"

/**
 * @brief bitmap structure initialization
 * 
 * @param map    bitmap
 * @param len_inbits length in bits
 * @param buff   bitmap buffer (size is in BYTES)
 */
void
bitmap_init(bitmap_t *map, uint32_t len_inbits, void *buff) {
    if (map == null)    panic("bitmap_init(): invalid map");
    map->len_inbits_ = len_inbits;
    map->buff_ = (uint8_t *)buff;
    map->prev_free_ = 0;
}

/**
 * @brief test the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param idx bit index
 * @retval true:  this bit is set
 * @retval false: this bit is clear
 */
bool bitmap_test(bitmap_t *map, idx_t idx) {
    if (map == null)    panic("bitmap_test(): invalid map");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_test(): invalid index");

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    return (map->buff_[byte] & ((uint8_t)true << bit));
}

/**
 * @brief set the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param idx bit index
 */
void
bitmap_set(bitmap_t *map, idx_t idx) {
    if (map == null)    panic("bitmap_set(): invalid map");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_set(): invalid index");

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    map->buff_[byte] |= ((uint8_t)true << bit);
}

/**
 * @brief clear the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param idx bit index
 */
void
bitmap_clear(bitmap_t *map, idx_t idx) {
    if (map == null)    panic("bitmap_clear(): invalid map");
    if (idx == INVALID_INDEX || idx >= map->len_inbits_)
        panic("bitmap_clear(): invalid index");

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    map->buff_[byte] &= ~((uint8_t)true << bit);
}

/**
 * @brief scan the bitmap to search an specific bit
 * (would not change the bitmap)
 * 
 * @param map bitmap
 * @return the first specified bit
 */
idx_t
bitmap_scan_empty(bitmap_t *map) {
    if (map == null)    panic("bitmap_scan_empty(): invalid map");
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
