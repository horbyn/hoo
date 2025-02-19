#include "user.h"

/**
 * @brief 系统调用入口
 * 
 * @param syscall_number 系统调用号
 * @param retval         返回值
 */
static void
syscall_entry(int syscall_number, void *retval) {
    __asm__ ("pushal\n\t"
        "movl (%%ebp),        %%ebx\n\t"
        "addl $0x8,           %%ebx\n\t"
        "movl (%%ebp),        %%ecx\n\t"
        "movl (%%ecx),        %%ecx\n\t"
        "int $0x80\n\t"
        "popal\n\t"
        "popl %%ebp\n\t"
        "ret" : : "d"(retval), "a"(syscall_number));
}

/**
 * @brief 创建文件或目录
 * 
 * @param filename 文件或目录名
 * @retval 0: 创建成功
 * @retval -1: 创建失败, 文件或目录已经存在
 */
int
sys_create(const char *filename) {
    int ret = -1;
    syscall_entry(SYS_CREATE, &ret);
    return ret;
}

/**
 * @brief 删除文件或目录
 * 
 * @param filename 文件或目录名
 * @retval 0: 删除成功
 * @retval -1: 删除失败, 找不到父目录
 * @retval -2: 删除失败, 父目录项类型无效
 * @retval -3: 删除失败, 文件或目录不存在
 */
int
sys_remove(const char *filename) {
    int ret = -1;
    syscall_entry(SYS_REMOVE, &ret);
    return ret;
}

/**
 * @brief 打开文件
 * 
 * @param filename 文件名
 * @return 文件描述符
 */
int
sys_open(const char *filename) {
    int fd = -1;
    syscall_entry(SYS_OPEN, &fd);
    return fd;
}

/**
 * @brief 关闭文件
 * 
 * @param fd 文件描述符
 */
void
sys_close(int fd) {
    syscall_entry(SYS_CLOSE, 0);
}

/**
 * @brief 读取文件
 * 
 * @param fd    文件描述符
 * @param buf   保存数据的缓冲区
 * @param count 要读取的字节数
 */
void
sys_read(int fd, void *buf, unsigned int count) {
    syscall_entry(SYS_READ, 0);
}

/**
 * @brief 写入文件
 * 
 * @param fd    文件描述符
 * @param buf   数据缓冲区
 * @param count 要写入的字节数
 */
void
sys_write(int fd, const void *buf, unsigned int count) {
    syscall_entry(SYS_WRITE, 0);
}

/**
 * @brief 格式化输出
 * 
 * @param format 格式化字符串
 */
void
sys_printf(const char *format, ...) {
    syscall_entry(SYS_PRINTF, 0);
}

/**
 * @brief 创建子进程
 * 
 * @return 子进程 id
 */
int
sys_fork(void) {
    int tid = -1;
    unsigned int bak_entry = 0;
    __asm__ ("movl 0x8(%%ebp), %0\n\t"
        "movl 0x4(%%ebp), %%eax\n\t"
        "movl %%eax, 0x8(%%ebp)"
        : "=c"(bak_entry));
    syscall_entry(SYS_FORK, &tid);
    __asm__ ("movl %0, 0x8(%%ebp)"
        : : "a"(bak_entry));
    return tid;
}

/**
 * @brief 父进程等待子进程终止
 */
void
sys_wait() {
    unsigned int bak_spinlock = 0;
    unsigned int temp_spinlock = 0;
    __asm__ ("movl 0x8(%%ebp), %0\n\t"
        "movl %1, 0x8(%%ebp)"
        : "=a"(bak_spinlock)
        : "c"(&temp_spinlock));
    syscall_entry(SYS_WAIT, 0);
    __asm__ ("movl %0, 0x8(%%ebp)"
        : : "a"(bak_spinlock));
}

/**
 * @brief 退出进程
 */
void
sys_exit() {
    syscall_entry(SYS_EXIT, 0);
}

/**
 * @brief 切换当前目录
 * 
 * @param dir 要切换的目录
 * 
 * @retval 0:  切换成功
 * @retval -1: 切换出错, 没有这个目录
 * @retval -2: 切换出错, 指定目录是一个文件
 * @retval -3: 切换出错, 目录名太长
 */
int
sys_cd(const char *dir) {
    int ret = -1;
    syscall_entry(SYS_CD, &ret);
    return ret;
}

/**
 * @brief 切换控制流
 * 
 * @param program 控制流入口
 */
void
sys_exec(const char *program) {
    syscall_entry(SYS_EXEC, 0);
}

/**
 * @brief 列出目录的所有文件，或者仅输出文件名
 * 
 * @param dir_or_file 指定一个目录名或文件名
 * 
 * @retval 0:  无异常
 * @retval -1: 没有这个目录或文件
 */
int
sys_ls(const char *dir_or_file) {
    int ret = -1;
    syscall_entry(SYS_LIST, &ret);
    return ret;
}

/**
 * @brief 内存动态分配
 * 
 * @param size 要分配的字节数
 * @return 内存地址
 */
void *
sys_alloc(unsigned int size) {
    void *ret = 0;
    syscall_entry(SYS_ALLOC, &ret);
    return ret;
}

/**
 * @brief 释放内存
 * 
 * @param ptr 要释放的内存地址
 */
void
sys_free(void *ptr) {
    syscall_entry(SYS_FREE, 0);
}

/**
 * @brief 获取工作目录
 * 
 * @param wd  保存结果的缓冲区
 * @param len 缓冲区长度
 * 
 * @retval 0:  成功
 * @retval -1: 出错，同时缓冲区会被填充 0
 */
int
sys_workingdir(char *wd, unsigned int len) {
    int ret = 0;
    syscall_entry(SYS_WORKINGDIR, &ret);
    return ret;
}
