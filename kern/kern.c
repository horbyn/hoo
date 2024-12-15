/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "kern.h"
#include "fs/builtins.h"
#include "fs/exec.h"
#include "module/conf.h"
#include "module/do_intr.h"
#include "module/driver.h"
#include "module/fs.h"
#include "module/io.h"
#include "module/log.h"
#include "module/mem.h"
#include "module/sched.h"
#include "sched/tasks.h"
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
 * @brief jump into the first ring3 thread
 */
static void
the_first_ring3(void) {
    sys_exec(BUILT_SHELL);
}

/**
 * @brief kernel initialization
 */
void
kern_init() {
    kinit_io();
    kinit_memory();
    kinit_config();
    kinit_tasks_system();

    // after that we could use dynamic memory allocation
    kinit_isr_idt();
    kinit_driver();
    kinit_fs();

    // after that we could enable interrupt
    enable_intr();
    load_builtins();
    kinit_log();
}

/**
 * @brief kernel run
 */
void
kern_exec(void) {
    tid_t result = fork(the_first_ring3);
    if (result != 0) {
        // the only thing for the ring0 kernel to do is to inspect
        //   whether there are the expired threads and kill them
        while (1)    kill();
    }
}
