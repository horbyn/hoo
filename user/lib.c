#include "lib.h"
#include "user.h"

/**
 * @brief 计算字符串长度
 * 
 * @param str 字符串
 * @return 长度
 */
uint32_t
strlen(const char *str) {
    uint32_t len = 0;
    while (*str++ != 0)    ++len;
    return len;
}

/**
 * @brief 比较两个字符串
 * 
 * @param a 字符串 a
 * @param b 字符串 b
 * @retval true:  相同
 * @retval false: 不同
 */
bool
strcmp(const char *a, const char *b) {
    uint32_t alen = strlen(a), blen = strlen(b);
    if (alen != blen)     return false;
    for (uint32_t i = 0; i < alen; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

/**
 * @brief 将 fill 填充到 buff 最多 bufflen 字节
 * 
 * @param buff    缓冲区
 * @param fill    填充字符
 * @param bufflen 填充长度
 */
void
memset(void *buff, uint8_t fill, uint32_t bufflen) {
    if (buff == null || bufflen == 0)    return;

    uint8_t *ptr = (uint8_t *)buff;
    while (bufflen--)    *ptr++ = fill;
}

/**
 * @brief 用 0 来填充缓冲区
 * 
 * @param buff    缓冲区
 * @param bufflen 填充长度
 */
void
bzero(void *buff, uint32_t bufflen) {
    memset(buff, 0, bufflen);
}

/**
 * @brief 拷贝内存
 * 
 * @param dst  目的地址
 * @param src  源地址
 * @param size 拷贝长度
 * @retval -1 出错
 * @retval 0  成功
 */
int
memmove(void *dst, const void *src, uint32_t size) {
    if (dst == null)    return -1;
    if (src == null || src == dst)    return 0;

    uint8_t *pdst = (uint8_t *)dst;
    const uint8_t *psrc = (uint8_t *)src;
    while (size--)    *pdst++ = *psrc++;

    return 0;
}

/**
 * @brief 动态分配内存
 * 
 * @param size 要分配的大小
 * @return 内存地址
 */
void *
alloc(uint32_t size) {
    return sys_alloc(size);
}

/**
 * @brief 释放内存
 * 
 * @param ptr 要释放的内存地址
 */
void
free(void *ptr) {
    sys_free(ptr);
}

/**
 * @brief 获取工作目录
 * 
 * @param wd  保存结果的缓冲区
 * @param len 缓冲区大小
 * 
 * @retval 0:  成功
 * @retval -1: 出错，同时缓冲区会填充 0
 */
int
workingdir(char *wd, uint32_t len) {
    return sys_workingdir(wd, len);
}
