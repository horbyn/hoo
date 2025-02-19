#pragma once
#ifndef __KERN_MODULE_LOG_H__
#define __KERN_MODULE_LOG_H__

#include "kern/fs/fs_stuff.h"

void klog_set(fd_t log_file);
void klog_write(const char *fmt, ...);

#endif
