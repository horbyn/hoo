#include "vm_kern.h"
#include "kern/panic.h"
#include "kern/utilities/spinlock.h"

static uint8_t __bmbuff_virmm[SIZE_BITMAP_VIRMM] __attribute__((aligned(16)));
static bitmap_t __bm_virmm;

/**
 * @brief 获取虚拟内存管理模块的 spinlock
 * 
 * @return spinlock 
 */
static spinlock_t *
vm_get_spinlock(void) {
    static spinlock_t vm_spinlock;
    return &vm_spinlock;
}

/**
 * @brief 初始化虚拟内存管理模块
 */
void
init_kern_virmm_bitmap() {
    spinlock_init(vm_get_spinlock());
    bitmap_init(&__bm_virmm, SIZE_BITMAP_VIRMM * BITS_PER_BYTE, __bmbuff_virmm);
}

/**
 * @brief 从内核 metadata 区域获取一个物理页
 * 
 * @return 物理页的虚拟地址
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
 * @brief 将一个物理页释放回内核 metadata 区域
 * 
 * @param va 要释放的物理页对应的虚拟地址
 */
void
vir_release_kern(void *va) {
    if (KERN_METADATA > (uint32_t)va || (uint32_t)va >= PG_MASK - MB4 + PGSIZE)
        panic("vir_release_kern(): not the kernel metadata");
    wait(vm_get_spinlock());
    bitmap_clear(&__bm_virmm, ((uint32_t)va - KERN_METADATA) / PGSIZE);
    signal(vm_get_spinlock());
}
