/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "bitmap.h"

/**
 * @brief test the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param len the length to be scanned
 * @param idx index
 * @retval true:  this bit is set
 * @retval false: this bit is clear
 */
bool
bitmap_test(void *map, uint32_t len, idx_t idx) {
    if (idx == INVALID_INDEX || idx >= len)
        panic("bitmap_test(): invalid index");

    uint8_t *m = (uint8_t *)map;

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    return (m[byte] & ((uint8_t)true << bit));
}

/**
 * @brief set the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param len the length to be scanned
 * @param idx index
 */
void
bitmap_set(void *map, uint32_t len, idx_t idx) {
    if (idx == INVALID_INDEX || idx >= len)
        panic("bitmap_set(): invalid index");

    uint8_t *m = (uint8_t *)map;

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    m[byte] |= ((uint8_t)true << bit);
}

/**
 * @brief clear the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param len the length to be scanned
 * @param idx index
 */
void
bitmap_clear(void *map, uint32_t len, idx_t idx) {
    if (idx == INVALID_INDEX || idx >= len)
        panic("bitmap_clear(): invalid index");

    uint8_t *m = (uint8_t *)map;

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    m[byte] &= ~((uint8_t)true << bit);
}

/**
 * @brief scan the bitmap to search empty bit
 * 
 * @param map bitmap
 * @param len the length to be scanned
 * @param from the beginning index to be scanned
 * @param is_set whether the specify bit is set
 * @return the bit first empty
 */
uint32_t
bitmap_scan(void *map, uint32_t len, idx_t from, bool is_set) {
    if (map == null || len == 0)
        panic("bitmap_scan(): bitmap is illegal");
    if (from == INVALID_INDEX || from >= len)
        panic("bitmap_scan(): invalid parameters");

    uint32_t bit = from;
    for (; bit < len; ++bit) {
        if (bitmap_test(map, len, bit) == is_set) {
            bitmap_set(map, len, bit);
            break;
        }
    }

    if (bit == len)
        panic("bitmap_scan(): no more free bits");
    return bit;
}
