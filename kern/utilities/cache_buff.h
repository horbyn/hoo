#pragma once
#ifndef __KERN_UTILITIES_CACHE_BUFF_H__
#define __KERN_UTILITIES_CACHE_BUFF_H__

#include "user/types.h"
#include "kern/fs/fs_stuff.h"

/**
 * @brief cache buffer
 */
typedef struct cache_buff {
    char       *buff_;
    uint32_t   capacity_;
    uint32_t   curlen_;
    fd_t       redirect_;
} cachebuff_t;

void cachebuff_set(cachebuff_t *cbuff, char *buff, uint32_t capacity);
void cachebuff_redirect(cachebuff_t *cbuff, fd_t redirect);
void cachebuff_write(cachebuff_t *cbuff, const char *string, uint32_t len);

#endif
