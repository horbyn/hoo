/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "fmngr.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief file manager initialization
 * 
 * @param fmngr file manager
 */
void
fmngr_init(fmngr_t *fmngr) {
    bzero(fmngr->fd_set_.buff_, fmngr->fd_set_.len_inbits_ / BITS_PER_BYTE);
    bitmap_init(&fmngr->fd_set_, 0, fmngr->fd_set_.buff_);
    bzero(fmngr->files_, sizeof(fd_t) * MAX_FILES_PER_TASK);
}

/**
 * @brief allocate the file descriptor
 *
 * @param fmngr file manager
 * @return file descriptor
 */
fd_t
fmngr_alloc(fmngr_t *fmngr) {
    if (fmngr == null)    panic("fmngr_alloc(): null pointer");
    return (fd_t)(bitmap_scan_empty(&fmngr->fd_set_));
}

/**
 * @brief release the files array index of file manager
 * 
 * @param fmngr file manager
 * @param fd    file descriptor
 */
void
fmngr_free(fmngr_t *fmngr, fd_t fd) {
    if (fmngr == null)    panic("fmngr_free(): null pointer");
    bitmap_clear(&fmngr->fd_set_, fd);
}

/**
 * @brief setup the file descriptor array of the file manager
 * 
 * @param fmngr file manager
 * @param fd    file descriptor
 * @param val   value
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
 * @brief get the files array element for specific index of file manager
 * 
 * @param fmngr file manager
 * @param fd    file descriptor
 * @return file array element
 */
fd_t
fmngr_files_get(fmngr_t *fmngr, fd_t fd) {
    if (fmngr == null || (fmngr != null && fmngr->files_ == null))
        panic("fmngr_files_get(): null pointer");
    if (fd > MAX_FILES_PER_TASK)    panic("fmngr_files_get(): invalid fd");
    return fmngr->files_[fd];
}
