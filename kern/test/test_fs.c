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

    kprintf("create direcoty: /usr/\n");
    files_create("/usr/");

    kprintf("\n\ncreate file: /usr/myfile.txt\n");
    files_create("/usr/myfile.txt");

    kprintf("\n\ndelete directory: /usr/\n");
    files_remove("/usr/");

    kprintf("create direcoty: /opt/\n");
    files_create("/opt/");

    kprintf("\n\ncreate file: /opt/toOpen.txt\n");
    files_create("/opt/toOpen.txt");

    fd_t fd = files_open("/opt/toOpen.txt");
    if (fd == -1)    panic("TEST FAILED: cannot open /opt/toOpen.txt");
    kprintf("\n\nopen file: /opt/toOpen.txt; %d\n", fd);

    files_write(fd, "Hello, world!", strlen("Hello, world!"));

    char buf[16];
    bzero(buf, sizeof(buf));
    files_read(fd, buf, sizeof(buf));
    kprintf("\n\nread file: /opt/toOpen.txt; %s\n", buf);

    kprintf("\n\nclose file: /opt/toOpen.txt\n");
    files_close(fd);

}
