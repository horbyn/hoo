/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "vm_kern.h"
#include "kern/panic.h"
#include "kern/utilities/spinlock.h"

static uint8_t __bmbuff_virmm[SIZE_BITMAP_VIRMM] __attribute__((aligned(16)));
static bitmap_t __bm_virmm;

/**
 * @brief get kernel virtual memory spinlock
 * 
 * @return spinlock 
 */
static spinlock_t *
vm_get_spinlock(void) {
    static spinlock_t vm_spinlock;
    return &vm_spinlock;
}

/**
 * @brief initialize virtual memory system
 */
void
init_kern_virmm_bitmap() {
    spinlock_init(vm_get_spinlock());
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
    wait(vm_get_spinlock());
    i = bitmap_scan_empty(&__bm_virmm);
    bitmap_set(&__bm_virmm, i);
    signal(vm_get_spinlock());
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
    wait(vm_get_spinlock());
    bitmap_clear(&__bm_virmm, ((uint32_t)va - KERN_METADATA) / PGSIZE);
    signal(vm_get_spinlock());
}
