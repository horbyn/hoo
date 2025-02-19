#include "builtins.h"
#include "exec.h"
#include "files.h"
#include "kern/panic.h"
#include "user/lib.h"
#include "kern/dyn/dynamic.h"

/**
 * @brief 将 filename 附加到 base_buff 后面
 * 
 * @param base_buff   base 文件名
 * @param append_buff 结果
 * @param filename    指定一个要追加的文件名
 */
static void
filename_append(const char *base_buff, char *append_buff, const char *filename) {
    if (base_buff == null || append_buff == null || filename == null)
        panic("filename_append(): null pointer");

    memmove(append_buff, base_buff, strlen(base_buff));
    memmove(append_buff + strlen(append_buff), filename, strlen(filename));
}

/**
 * @brief 将内置命令转换为文件
 * 
 * @param filename 文件名
 * @param addr     内置命令的内存地址
 * @param len      内置命令的二进制数据长度
 */
static void
builtin_to_file(const char *filename, void *addr, uint32_t len) {
    if (filename == null || addr == null || len == 0)    return;

    char *specific_file = dyn_alloc(64);
    bzero(specific_file, 64);
    filename_append(DIR_LOADER, specific_file, filename);
    if (files_create(specific_file) == 0) {
        fd_t fd = files_open(specific_file);
        if (fd == -1)    return;
        files_write(fd, addr, len);
        files_close(fd);
    }

    dyn_free(specific_file);
}

/**
 * @brief 加载内置命令
 */
void
load_builtins(void) {
    files_create(DIR_LOADER);

#ifdef __BASE_BUILTIN_SH
    builtin_to_file(BUILT_SHELL, (void *)__BASE_BUILTIN_SH,
        (uint32_t)__END_BUILTIN_SH - (uint32_t)__BASE_BUILTIN_SH);
    builtin_to_file(BUILT_CD, (void *)__BASE_BUILTIN_CD,
        (uint32_t)__END_BUILTIN_CD - (uint32_t)__BASE_BUILTIN_CD);
    builtin_to_file(BUILT_LS, (void *)__BASE_BUILTIN_LS,
        (uint32_t)__END_BUILTIN_LS - (uint32_t)__BASE_BUILTIN_LS);
    builtin_to_file(BUILT_PWD, (void *)__BASE_BUILTIN_PWD,
        (uint32_t)__END_BUILTIN_PWD - (uint32_t)__BASE_BUILTIN_PWD);
    builtin_to_file(BUILT_MKDIR, (void *)__BASE_BUILTIN_MKDIR,
        (uint32_t)__END_BUILTIN_MKDIR - (uint32_t)__BASE_BUILTIN_MKDIR);
    builtin_to_file(BUILT_TOUCH, (void *)__BASE_BUILTIN_TOUCH,
        (uint32_t)__END_BUILTIN_TOUCH - (uint32_t)__BASE_BUILTIN_TOUCH);
    builtin_to_file(BUILT_RM, (void *)__BASE_BUILTIN_RM,
        (uint32_t)__END_BUILTIN_RM - (uint32_t)__BASE_BUILTIN_RM);
#endif
}
