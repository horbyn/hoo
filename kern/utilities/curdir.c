/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "curdir.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief the current directory initialization
 * 
 * @param curdir the current directory
 * @param dir    the directory buffer
 * @param dirlen the buffer length
 */
void
curdir_init(curdir_t *curdir, char *dir, uint32_t dirlen) {
    if (curdir == null)    panic("curdir_init(): null pointer");
    if (dir != 0) {
        curdir->dir_ = dir;
        bzero(curdir->dir_, dirlen);
    } else    curdir->dir_ = 0;
    if (dirlen != 0)    curdir->dirlen_ = dirlen;
    else    curdir->dirlen_ = 0;
}

/**
 * @brief get the current directory
 *
 * @param curdir  curdir object
 * @param path    the result
 * @param pathlen the path buffer size
 *
 * @retval 0: succeed
 * @retval -1: failed, and the buffer will be fill in zero
 */
int
curdir_get(const curdir_t *curdir, char *path, uint32_t pathlen) {
    if (curdir == null)    panic("curdir_get(): null pointer");

    uint32_t acc = 0;
    const char *worker = 0;
    for (int i = 0; i < MAX_OPEN_DIR; ++i) {
        worker = curdir->dir_ + i * DIRITEM_NAME_LEN;
        if (worker[0] == 0)    break;

        uint32_t len = strlen(worker);
        if (acc + len > pathlen) {
            bzero(path, pathlen);
            return -1;
        } else {
            memmove(path + acc, worker, len);
            acc += len;
        }
    }

    path[acc] = 0;
    return 0;
}

/**
 * @brief setup the current directory
 * @note setup the current directory whatever the path is exists
 * @param curdir curdir object
 * @param path the path to be set
 * 
 * @retval 0: succeed
 * @retval -1: failed, and path is overflowed
 */
int
curdir_set(curdir_t *curdir, const char *path) {
    if (curdir == null)    panic("curdir_set(): null pointer");

    char *worker = 0;
    int i = 0, j = 0;
    bzero(curdir->dir_, curdir->dirlen_);

    for (; i < MAX_OPEN_DIR; ++i) {
        if (path[j] == 0)    break;
        worker = curdir->dir_ + i * DIRITEM_NAME_LEN;

        for (;; ++j) {
            if (path[j] == DIR_SEPARATOR) {
                ++j;
                break;
            } else    *worker++ = path[j];
        }
        *worker++ = DIR_SEPARATOR;
        *worker = 0;
    } // end for()
    if (i == MAX_OPEN_DIR)    return -1;

    return 0;
}

/**
 * @brief copy current directory
 * 
 * @param dst destination
 * @param src source
 */
void
curdir_copy(curdir_t *dst, const curdir_t *src) {
    if (dst == null || src == null)    panic("curdir_copy(): null pointer");

    if (dst->dir_ != 0) {
        if (src->dir_ != 0)    memmove(dst->dir_, src->dir_, src->dirlen_);
        else    dst->dir_[0] = 0;
    }
}

/**
 * @brief get the parent filename
 * 
 * @note
 * for "/usr/bin/" would get parent "/usr/" and child "bin"
 * @note
 * for "/usr/bin" would also get parent "/usr/" and child "bin"
 * @note
 * for "/" would get parent "/" and child "" (null pointer)
 * 
 * @param path_to_parent the specific filename (will change to parent at the end)
 * @param child          the child name buffer (if exists)
 */
void
get_parent_child_filename(char *path_to_parent, char *cur) {
    if (path_to_parent == 0)    panic("get_parent_child_filename(): null pointer");
    if (path_to_parent[0] != DIRNAME_ROOT_ASCII)
        panic("get_parent_child_filename(): not a absolute directory");

    int separator = -1;
    uint32_t path_sz = strlen(path_to_parent);
    for (uint32_t i = path_sz - 1; i >= 0; --i) {
        if (path_to_parent[i] == DIRNAME_ROOT_ASCII && i != path_sz - 1) {
            separator = i;
            break;
        }
        if (i == 0)    break;
    }

    if (separator == -1) {
        // corresponding to the case like "/"
        path_to_parent[0] = DIRNAME_ROOT_ASCII;
        path_to_parent[1] = 0;
        if (cur != 0) {
            cur[0] = 0;
        }
    } else {
        if (cur != 0) {
            memmove(cur, path_to_parent + separator + 1, path_sz - separator - 1);
            uint32_t curlen = strlen(cur);
            if (cur[curlen - 1] == DIRNAME_ROOT_ASCII)    cur[curlen - 1] = 0;
        }

        path_to_parent[separator + 1] = 0;
    }
}
