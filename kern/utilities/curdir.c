#include "curdir.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief 初始化当前目录结构
 * 
 * @param curdir curdir 对象
 * @param dir    目录结构缓冲区
 * @param dirlen 缓冲区长度
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
 * @brief 获取当前目录的目录结构
 *
 * @param curdir  curdir 对象
 * @param path    记录结果的缓冲区
 * @param pathlen 缓冲区长度
 *
 * @retval 0:  成功
 * @retval -1: 出错，同时缓冲区全部填充 0
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
 * @brief 填充 curdir 对象
 * @note 不检测目录结构是否存在
 * @param curdir curdir 对象
 * @param path   要设置的路径
 * 
 * @retval 0:  成功
 * @retval -1: 出错，因为路径太长了
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
 * @brief 拷贝 curdir 对象
 * 
 * @param dst 目的对象
 * @param src 源对象
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
 * @brief 获取父目录名（与子目录名）
 * 
 * @note 对于 "/usr/bin/" 会得到父目录 "/usr/" 和子目录 "bin"
 * @note 对于 "/usr/bin" 会得到父目录 "/usr/" 和子目录 "bin"
 * @note 对于 "/" 会得到父目录 "/" 和子目录 "" (null pointer)
 * 
 * @param path_to_parent 指定一个文件名（最终会变成父目录名）
 * @param child          子目录名（如果给出）
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
        // 对应像 "/" 这样的情况
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
