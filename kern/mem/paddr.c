/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#include "paddr.h"

static uint8_t __bitmap_phymm[SIZE_BITMAP_PHYMM4G] = { 0 };

/**
 * @brief allocate a physical page
 * 
 * @return a physical address
 */
void *
phy_alloc_page() {
    uint32_t i = bitmap_scan(__bitmap_phymm, sizeof(__bitmap_phymm), 0x100, false);
    bitmap_set(__bitmap_phymm, sizeof(__bitmap_phymm), i);
    return (void *)(i <<= 12);
}

void
phy_release_page(void *page_phy_addr) {
    if (page_phy_addr == null)    return;
    uint32_t i = ((uint32_t)page_phy_addr) >> 12;
    bitmap_clear(__bitmap_phymm, sizeof(__bitmap_phymm), i);
}
