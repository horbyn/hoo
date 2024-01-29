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
#include "kern/driver/ata_driver.h"
#include "kern/lib/list.h"
#include "kern/lib/lib.h"

#define NODE_NUM 10                                         // temporary node for blocks io

/**
 * @brief blocks
 */
typedef struct blocks_t {
    void            *block_addr_;                           // block address
    lba_index_t     lba_;                                   // lba
} __attribute__((packed)) blocks_t;

/**
 * @brief blocks list
 */
typedef list_t blocks_list_t;

void blocks_list_init(blocks_list_t *list);
void set_blocks(blocks_t *block, void *block_addr, lba_index_t lba);
void blocks_append(blocks_list_t *list, const blocks_t *block);
void blocks_rw_disk(blocks_list_t *list, ata_cmd_t cmd);

#endif
