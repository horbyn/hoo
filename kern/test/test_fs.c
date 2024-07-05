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
    clear_screen();
    kprintf("> TEST_FS <\n");

    kprintf("create direcoty: /usr\n");
    files_create(INODE_TYPE_DIR, "/usr");

    kprintf("\n\ncreate file: /usr/myfile.txt\n");
    files_create(INODE_TYPE_FILE, "/usr/myfile.txt");
}
