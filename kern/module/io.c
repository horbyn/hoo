#include "io.h"
#include "kern/utilities/format.h"
#include "kern/driver/cga/cga.h"

/**
 * @brief IO 模块初始化
 */
void
kinit_io(void) {
    cga_clear();
    cga_init();
    cga_set_attribute(SCBUFF_COLOR_WHITE, SCBUFF_STYLE_LIGHT);
}

/**
 * @brief 格式化输出
 * 
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void
kprintf(const char *fmt, ...) {
    va_list va;
    VA_START(va, fmt);
    format(fmt, va, (void *)FD_STDOUT);
    VA_END(va);
}
