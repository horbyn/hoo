/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm_kern.h"

static uint8_t __bmbuff_virmm[SIZE_BITMAP_VIRMM] __attribute__((aligned(16)));
static bitmap_t __bm_virmm;

/**
 * @brief initialize virtual memory system
 */
void
init_kern_virmm_bitmap() {
    bitmap_init(&__bm_virmm, SIZE_BITMAP_VIRMM * BITS_PER_BYTE, __bmbuff_virmm);
}

/**
 * @brief get one metadata virtual page
 * 
 * @return virtual address
 */
void *
vir_alloc_kern(void) {
    int i = 0;

    i = bitmap_scan_empty(&__bm_virmm);
    bitmap_set(&__bm_virmm, i);
    return (void *)(KERN_METADATA + i * PGSIZE);
}

/**
 * @brief release one metadata virtual page
 * 
 * @param va: virtual address to be released
 */
void
vir_release_kern(void *va) {
    if (KERN_METADATA > (uint32_t)va || (uint32_t)va >= PG_MASK - MB4 + PGSIZE)
        panic("vir_release_kern(): not the kernel metadata");
    bitmap_clear(&__bm_virmm, ((uint32_t)va - KERN_METADATA) / PGSIZE);
}
