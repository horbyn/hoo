#include "fmngr.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief 文件管理器初始化
 * 
 * @param fmngr 文件管理器
 */
void
fmngr_init(fmngr_t *fmngr) {
    bzero(fmngr->fd_set_.buff_, fmngr->fd_set_.len_inbits_ / BITS_PER_BYTE);
    bitmap_init(&fmngr->fd_set_, 0, fmngr->fd_set_.buff_);
    bzero(fmngr->files_, sizeof(fd_t) * MAX_FILES_PER_TASK);
}

/**
 * @brief 让文件管理器中分配一个可用的文件描述符
 *
 * @param fmngr 文件管理器
 * @return 文件描述符
 */
fd_t
fmngr_alloc(fmngr_t *fmngr) {
    if (fmngr == null)    panic("fmngr_alloc(): null pointer");
    return (fd_t)(bitmap_scan_empty(&fmngr->fd_set_));
}

/**
 * @brief 文件管理器回收文件描述符
 * 
 * @param fmngr 文件管理器
 * @param fd    文件描述符
 */
void
fmngr_free(fmngr_t *fmngr, fd_t fd) {
    if (fmngr == null)    panic("fmngr_free(): null pointer");
    bitmap_clear(&fmngr->fd_set_, fd);
}

/**
 * @brief 向文件管理器中特定的索引设置元素
 * 
 * @param fmngr 文件管理器
 * @param fd    索引（文件描述符）
 * @param val   要设置的元素
 */
void
fmngr_files_set(fmngr_t *fmngr, fd_t fd, fd_t val) {
    if (fmngr == null || (fmngr != null && fmngr->files_ == null))
        panic("fmngr_files_set(): null pointer");
    if (fd > MAX_FILES_PER_TASK)    panic("fmngr_files_set(): invalid fd");
    if (val == INVALID_INDEX)    panic("fmngr_files_set(): invalid index");
    fmngr->files_[fd] = val;
}

/**
 * @brief 从文件管理器的特定索引中获取元素
 * 
 * @param fmngr 文件管理器
 * @param fd    索引（文件描述符）
 * @return 文件数组元素
 */
fd_t
fmngr_files_get(fmngr_t *fmngr, fd_t fd) {
    if (fmngr == null || (fmngr != null && fmngr->files_ == null))
        panic("fmngr_files_get(): null pointer");
    if (fd > MAX_FILES_PER_TASK)    panic("fmngr_files_get(): invalid fd");
    return fmngr->files_[fd];
}
