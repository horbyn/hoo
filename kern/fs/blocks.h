/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_BLOCKS_H__
#define __KERN_FS_BLOCKS_H__

#include "fs_stuff.h"
#include "kern/lib/list.h"

#define NODE_NUM 10                                         // temporary node for blocks io

/**
 * @brief blocks
 */
typedef struct blocks_t {
    void            *block_addr_;                           // block address
    lba_index_t     lba_;                                   // lba
} __attribute__((packed)) blocks_t;

void blocks_init(void);

#endif
