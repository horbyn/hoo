#include "format.h"
#include "user/types.h"
#include "kern/fs/files.h"
#include "kern/hoo/hoo.h"
#include "kern/driver/cga/cga.h"
#include "kern/utilities/format.h"
#include "kern/utilities/cache_buff.h"
#include "kern/module/io.h"
#include "user/lib.h"

/**
 * @brief 输出一个字符
 * @param ch       字符
 * @param redirect 输出重定向
 */
static void
outputc(char ch, void *redirect) {
    if (redirect == (void *)FD_STDOUT || redirect == (void *)FD_STDERR)
        files_write(FD_STDOUT, &ch, 1);
    else    cachebuff_write((cachebuff_t *)redirect, &ch, 1);
}

/**
 * @brief 输出一个字符串
 * @param str      字符串
 * @param redirect 输出重定向
 */
static void
outputstr(const char *str, void *redirect) {
    if (redirect == (void *)FD_STDOUT || redirect == (void *)FD_STDERR)
        files_write(FD_STDOUT, str, strlen(str));
    else    cachebuff_write((cachebuff_t *)redirect, str, strlen(str));
}

/**
 * @brief 输出八进制
 * @param digit    八进制数字
 * @param base     基数
 * @param redirect 输出重定向
 */
static void
outputdig(uint32_t digit, uint8_t base, void *redirect) {

    static char __arr_dig[32];
    int i = 0;

    do {
        int remaider = digit % base;
        if (remaider < 10)    __arr_dig[i] = (char)('0' + remaider);
        else    __arr_dig[i] = (char)('a' + (remaider - 10));
        digit /= base;
        ++i;
    } while (digit);

    while (i)    outputc(__arr_dig[--i], redirect);
}

/**
 * @brief 格式化处理
 * 
 * @param fmt      格式化字符串
 * @param args     可变参数
 * @param redirect 输出重定向
 */
void
format(const char *fmt, va_list args, void *redirect) {

    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            outputc(*fmt, redirect);
            continue;
        }

        // 如果最后一个字符是 % 则忽略
        if (*(fmt + 1) == 0)    return;
        else    ++fmt;

        // %% %c %s %d %x(%X) %p
        switch(*fmt) {
        case '%': outputc('%', redirect); break;
        case 'c': VA_ARG(args, char); outputc(*((char *)args), redirect); break;
        case 's':
            VA_ARG(args, const char *);
            uint32_t addr = *((uint32_t *)args);
            outputstr((const char *)addr, redirect);
            break;
        case 'd': VA_ARG(args, int); outputdig(*((int *)args), 10, redirect); break;
        case 'p':
        case 'X':
        case 'x':
            VA_ARG(args, uint32_t);
            outputdig(*((uint32_t *)args), 16, redirect);
            break;
        default:
            // TODO: %f
            VA_ARG(args, POINTER_SIZE);
            outputc('%', redirect);
            outputc(*fmt, redirect);
            break;
        }
    }

}
