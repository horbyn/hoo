#pragma once
#ifndef __KERN_MODULE_MEM_H__
#define __KERN_MODULE_MEM_H__

#include "user/types.h"

/**
 * @brief 物理内存信息
 */
typedef struct mem_info {
    uint32_t base_;
    uint32_t length_;
} mminfo_t;

void kinit_memory();

#endif
