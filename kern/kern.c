/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern.h"
#include "module/conf.h"
#include "module/do_intr.h"
#include "module/driver.h"
#include "module/fs.h"
#include "module/io.h"
#include "module/log.h"
#include "module/mem.h"
#include "module/sched.h"
#include "user/lib.h"
#include "user/user.h"

/**
 * @brief initialize system log module
 */
static void
kinit_log(void) {
    files_create("/sys/");

    static char *SYS_LOG_NAME = "/sys/sys.log";
    files_create(SYS_LOG_NAME);
    // DO NOT release the file descriptor
    klog_set(files_open(SYS_LOG_NAME));
}

/**
 * @brief kernel initialization
 */
void
kern_init() {
    kinit_io();
    kinit_config();
    kinit_isr_idt();
    kinit_driver();
    kinit_memory();
    kinit_tasks_system();
    // after that we could use dynamic memory allocation
    kinit_fs();
    kinit_log();
}

/**
 * @brief kernel run
 */
void
kern_exec(void) {
    enable_intr();
}
