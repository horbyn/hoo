/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "cache_buff.h"
#include "kern/panic.h"
#include "kern/driver/cga/cga.h"
#include "kern/fs/files.h"
#include "user/lib.h"

/**
 * @brief the cache buffer writes to file
 * 
 * @param cbuff cache buffer
 */
static void
cachebuff_flush(cachebuff_t *cbuff) {
    if (cbuff == null)    panic("scbuff_clear(): null pointer");
    while (cbuff->curlen_ >= BYTES_SECTOR) {
        files_write(cbuff->redirect_, cbuff->buff_, BYTES_SECTOR);
        cbuff->curlen_ -= BYTES_SECTOR;
    }
}

/**
 * @brief setup cache buffer
 * 
 * @param cbuff    cache buffer
 * @param buff     buffer
 * @param capacity buffer capacity
 */
void
cachebuff_set(cachebuff_t *cbuff, char *buff, uint32_t capacity) {
    if (cbuff == null || (cbuff != null && buff == 0))
        panic("cachebuff_set(): null pointer");
    cbuff->buff_ = buff;
    cbuff->capacity_ = capacity;
    cbuff->curlen_ = 0;
    cbuff->redirect_ = INVALID_INDEX;
}

/**
 * @brief setup redirection
 * 
 * @param cbuff    cache buffer
 * @param redirect redirection file
 */
void
cachebuff_redirect(cachebuff_t *cbuff, fd_t redirect) {
    if (cbuff == null)    panic("cachebuff_set(): null pointer");
    cbuff->redirect_ = redirect;
}

/**
 * @brief write to cache buffer
 * 
 * @param cbuff  cache buffer
 * @param string the string
 * @param len    the length
 */
void
cachebuff_write(cachebuff_t *cbuff, const char *string, uint32_t len) {
    if (cbuff == null || (cbuff != null && string == 0))
        panic("cachebuff_write(): null pointer");
    if (cbuff->redirect_ == INVALID_INDEX) {
        // DO NOT write to files before file system initialization finishing,
        //   but detect whether the cache buffer is full. If so, discard part
        //   of the beginning logs
        if (cbuff->curlen_ + len > cbuff->capacity_) {
            memmove(cbuff->buff_, cbuff->buff_ + len, len);
            cbuff->curlen_ -= len;
        }
    } else {
        // write to files once the cache buffer is 512 Bytes enough
        if (cbuff->curlen_ >= BYTES_SECTOR)    cachebuff_flush(cbuff);
    }
    while (len--)    cbuff->buff_[cbuff->curlen_++] = *string++;
}
