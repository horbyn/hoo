/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "fmngr.h"

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
    if (fmngr == null)    panic("fmngr_files_set(): null pointer");
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
    if (fmngr == null)    panic("fmngr_files_get(): null pointer");
    if (fd > MAX_FILES_PER_TASK)    panic("fmngr_files_get(): invalid fd");
    return fmngr->files_[fd];
}
