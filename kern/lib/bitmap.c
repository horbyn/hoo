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
 * @param idx index
 * @retval true:  this bit is set
 * @retval false: this bit is clear
 */
bool
bitmap_test(void *map, idx_t idx) {
    uint8_t *m = (uint8_t *)map;

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    return (m[byte] & ((uint8_t)true << bit));
}

/**
 * @brief set the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param idx index
 */
void
bitmap_set(void *map, idx_t idx) {
    uint8_t *m = (uint8_t *)map;

    idx_t byte = idx / BITS_PER_BYTE;
    idx_t bit = idx % BITS_PER_BYTE;

    m[byte] |= ((uint8_t)true << bit);
}

/**
 * @brief clear the specific bit of a bitmap
 * 
 * @param map bitmap
 * @param idx index
 */
void
bitmap_clear(void *map, idx_t idx) {
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
 * @return the bit first empty
 */
uint32_t
bitmap_scan(void *map, uint32_t len) {
    uint32_t bit = 0;

    for (; bit < len; ++bit) {
        if (bitmap_test(map, bit) == false) {
            bitmap_set(map, bit);
            break;
        }
    }

    if (bit == len)
        panic("bitmap_scan(): no more free bits");
    return bit;
}
