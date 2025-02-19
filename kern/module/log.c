#include "log.h"
#include "kern/hoo/hoo.h"
#include "kern/utilities/format.h"

/**
 * @brief 设置内核日志文件
 * 
 * @param log_file 日志文件描述符
 */
void
klog_set(fd_t log_file) {
    get_hoo_cache_buff()->redirect_ = log_file;
}

/**
 * @brief 记录系统日志
 * @note 系统日志在内核初始化之前记录，会被暂存在缓存中，内核文件系统初始化后才会被重定向到文件
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void
klog_write(const char *fmt, ...) {
    va_list va;
    VA_START(va, fmt);
    format(fmt, va, get_hoo_cache_buff());
    VA_END(va);
}
