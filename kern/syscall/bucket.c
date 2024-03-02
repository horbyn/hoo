/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "bucket.h"

/*static bucket_desc_t __bucket_desc[] = {
    { 8, null },
    { 16, null },
    { 32, null },
    { 64, null },
    { 128, null },
    { 256, null },
    { 512, null },
    { 1024, null },
    { 2048, null },
    { 4096, null },
    { 0, null }
};*/

/**
 * @brief allocate specify size of memory within one page
 * 
 * @param n size
 * @return the physical address
 */
/*static void *
phy_alloc_under_one_page(uint32_t n) {
    if (n > PGSIZE)    panic("phy_alloc_under_one_page(): parameter invalid");

    uint32_t i = 0;
    for (; i < NELEMS(__bucket_desc); ++i) {
        if (__bucket_desc[i].capacity_ >= n)    break;
    }
    if (i == NELEMS(__bucket_desc))
        panic("phy_alloc_under_one_page(): require too much");

    return null;
}*/
