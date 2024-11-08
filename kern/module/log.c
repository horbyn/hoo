/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "log.h"
#include "kern/hoo/hoo.h"
#include "kern/utilities/format.h"

/**
 * @brief setup kernel log file
 * 
 * @param log_file log file descriptor
 */
void
klog_set(fd_t log_file) {
    get_hoo_cache_buff()->redirect_ = log_file;
}

/**
 * @brief record system log
 * @note system log could be recorded before kernel initialization, which will
 *       be stashed in a cache buffer. The file redirection only work well after
 *       the kernel file system initialization
 * @param fmt formatting string
 * @param ... variadic parameters
 */
void
klog_write(const char *fmt, ...) {
    va_list va;
    VA_START(va, fmt);
    format(fmt, va, get_hoo_cache_buff());
    VA_END(va);
}
