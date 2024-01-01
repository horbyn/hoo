/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_FS_FS_OPER_H__
#define __KERN_FS_FS_OPER_H__

#include "fs_stuff.h"
#include "device/ata/ata.h"
#include "kern/debug.h"
#include "kern/lib/lib.h"

void setup_super_block(super_block_t *, uint32_t, uint32_t);
void setup_inode(uint32_t, uint32_t);
void setup_disk_layout(void);

#endif
