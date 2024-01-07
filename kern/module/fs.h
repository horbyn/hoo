/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __KERN_MODULE_FS_H__
#define __KERN_MODULE_FS_H__

#include "kern/fs/super_block.h"
#include "kern/fs/inodes.h"
#include "kern/fs/free.h"
#include "kern/fs/dir.h"
#include "kern/driver/ata_driver.h"

void init_fs(void);

#endif
