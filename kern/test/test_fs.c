/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "test.h"
#include "kern/fs/files.h"

/**
 * @brief file system testing
 */
void
test_fs() {
    kprintf("create direcoty: /usr");
    files_create(INODE_TYPE_DIR, "/usr");
}
