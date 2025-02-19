#pragma once
#ifndef __KERN_MEM_VADDR_H__
#define __KERN_MEM_VADDR_H__

#include "kern/utilities/list.h"

/**
 * @brief 虚拟地址定义
 */
typedef struct vaddr {
    uint32_t va_;
    // 在 va 后面的页框数量
    uint32_t length_;
} vaddr_t;

/**
 * @brief 虚拟地址空间
 */
typedef struct vaddr_space {
    list_t             list_;
    uint32_t           begin_;
    uint32_t           end_;
    struct vaddr_space *next_;
} vspace_t;

void vaddr_set(vaddr_t *vaddr, uint32_t addr, uint32_t length);
void vspace_set(vspace_t *vs, list_t *ls, uint32_t begin, uint32_t end, vspace_t *next);

#endif
