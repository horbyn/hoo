#include "cache_buff.h"
#include "kern/panic.h"
#include "kern/driver/cga/cga.h"
#include "kern/fs/files.h"
#include "user/lib.h"

/**
 * @brief 要写入文件的 cache buffer
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
 * @brief 填充 cache buffer
 * 
 * @param cbuff    cache buffer
 * @param buff     缓冲区
 * @param capacity 缓冲区容量
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
 * @brief 设置重定向
 * 
 * @param cbuff    cache buffer
 * @param redirect 重定向的文件描述符
 */
void
cachebuff_redirect(cachebuff_t *cbuff, fd_t redirect) {
    if (cbuff == null)    panic("cachebuff_set(): null pointer");
    cbuff->redirect_ = redirect;
}

/**
 * @brief 写入 cache buffer
 * 
 * @param cbuff  cache buffer
 * @param string 要写入的字符串
 * @param len    字符串长度
 */
void
cachebuff_write(cachebuff_t *cbuff, const char *string, uint32_t len) {
    if (cbuff == null || (cbuff != null && string == 0))
        panic("cachebuff_write(): null pointer");
    if (cbuff->redirect_ == INVALID_INDEX) {
        // 如果缓存区满了则丢弃前面的日志
        if (cbuff->curlen_ + len > cbuff->capacity_) {
            memmove(cbuff->buff_, cbuff->buff_ + len, len);
            cbuff->curlen_ -= len;
        }
    } else {
        // 每次当 cache buffer 到达 512B 就写入文件
        if (cbuff->curlen_ >= BYTES_SECTOR)    cachebuff_flush(cbuff);
    }
    while (len--)    cbuff->buff_[cbuff->curlen_++] = *string++;
}
