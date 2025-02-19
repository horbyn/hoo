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
 * @brief 初始化系统日志
 */
static void
kinit_log(void) {
    files_create("/sys/");

    static char *SYS_LOG_NAME = "/sys/sys.log";
    files_create(SYS_LOG_NAME);
    // 不释放这个文件描述符
    klog_set(files_open(SYS_LOG_NAME));
}

/**
 * @brief 跳入 ring3 进程
 */
static void
the_first_ring3(void) {
    sys_exec(BUILT_SHELL);
}

/**
 * @brief 内核初始化
 */
void
kern_init() {
    kinit_io();
    kinit_memory();
    kinit_config();
    kinit_tasks_system();

    // 在调度模块初始化完成后就可以使用动态内存分配了
    kinit_isr_idt();
    kinit_driver();
    kinit_fs();

    // 在所有事情初始化完成后就可以开中断了
    enable_intr();
    load_builtins();
    kinit_log();
}

/**
 * @brief 运行内核
 */
void
kern_exec(void) {
    tid_t result = fork(the_first_ring3);
    if (result != 0) {
        // ring0 内核做的唯一一件事是检查是否有 expired 进程，有就杀掉
        while (1)    kill();
    }
}
