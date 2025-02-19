#include "cga.h"
#include "user/lib.h"
#include "kern/utilities/spinlock.h"

/**
 * @brief 获取 CGA spinlock
 * 
 * @return spinlock 
 */
static spinlock_t *
cga_get_spinlock(void) {
    static spinlock_t cga_spinlock;
    return &cga_spinlock;
}

/**
 * @brief 获取 CGA 属性
 * 
 * @return CGA 属性
 */
static uint8_t *
cga_get_attribute(void) {
    static uint8_t cga_attr = 0;
    return &cga_attr;
}

/**
 * @brief 屏幕回滚
 */
static void
cga_scroll_back(void) {
    uint16_t beg = TO_POS(1, 0);
    uint16_t *vm = (uint16_t *)VIDEO_MEM;
    memmove(vm, vm + beg, (LASTLINE_END - beg + 1) * sizeof(uint16_t));

    // 最后一行全部填充空格
    for (uint32_t i = LASTLINE_BEG; i <= LASTLINE_END; ++i)
        vm[i] = WHITH_CHAR;
}

/**
 * @brief 获取光标
 * 
 * @return CGA 坐标
 */
static uint16_t
cga_cursor_get(void) {
    uint8_t low = 0;
    uint16_t hig = 0;
    outb(CGA_CURSOR_LOW, CGA_REG_INDEX);
    low = inb(CGA_REG_DATA);
    outb(CGA_CURSOR_HIGH, CGA_REG_INDEX);
    hig = inb(CGA_REG_DATA);

    return ((hig <<= 8) | low);
}

/**
 * @brief 设置光标
 * 
 * @param pos CGA 坐标
 */
static void
cga_cursor_set(uint16_t pos) {
    // 低 8 位
    outb(CGA_CURSOR_LOW, CGA_REG_INDEX);
    outb((uint8_t)(pos & 0xff), CGA_REG_DATA);

    // 高 8 位
    outb(CGA_CURSOR_HIGH, CGA_REG_INDEX);
    outb((uint8_t)((pos & 0xff00) >> 8), CGA_REG_DATA);
}

/**
 * @brief 初始化 CGA
 */
void
cga_init(void) {
    spinlock_init(cga_get_spinlock());
}

/**
 * @brief 清屏
 */
void
cga_clear(void) {
    wait(cga_get_spinlock());
    uint16_t *vm = (uint16_t *)VIDEO_MEM;
    for (uint32_t i = 0; i < (CGA_WIDTH * CGA_HIGH); ++i)
        vm[i] = WHITH_CHAR;
    cga_cursor_set(0);
    signal(cga_get_spinlock());
}

/**
 * @brief 设置 CGA 属性
 * 
 * @param color  CGA 颜色
 * @param bright CGA 亮度
 */
void
cga_set_attribute(color_t color, bright_t bright) {

    /*
     * 下面是 CGA 规范，这里只设置前景色
     * 
     * Bit 76543210
     *     ||||||||
     *     |||||^^^-前景色
     *     ||||^----前景色亮度
     *     |^^^-----背景色
     *     ^--------背景色亮度或者文字闪烁
     */

    *(cga_get_attribute()) = (uint8_t)0 | ((uint8_t)bright << 3) | (uint8_t)color;
}

/**
 * @brief 输出一个字符
 * @param ch   要输出的字符
 * @param attr CGA 属性
 */
static void
cga_putc(char ch, uint8_t attr) {
    uint16_t *vm = (uint16_t*)VIDEO_MEM;
    uint16_t ch_attr = (uint16_t)attr << 8;
    uint16_t pos = cga_cursor_get();

    // 特例: `\b` `\t` `\n`
    if (ch == '\b') {
        if (!(TO_ROW(pos) == 0 && TO_COL(pos) == 0)) {
            *(vm + (--pos)) = (ch_attr | ' ');
            uint16_t spaces = 0;
            for (uint16_t i = pos - 1; (vm[i] & 0xff) == ' ' || i == 0; --i)
                ++spaces;
            if (spaces > 1)    pos -= spaces;
        }

    } else if (ch == '\t') {
        // 模除 4
        int spaces = SIZE_TAG - (pos % SIZE_TAG);

        // 填充空格
        for (uint32_t i = 0; i < spaces; ++i, ++pos)
            *(vm + pos) = WHITH_CHAR;

    } else if (ch == '\n') {
        if (LASTLINE_BEG <= pos && pos <= LASTLINE_END) {
            pos = LASTLINE_BEG;
            cga_scroll_back();
        } else    pos = TO_POS(TO_ROW(pos) + 1, 0);

    } else {
        *(vm + pos++) = (ch_attr | ch);
        if (pos == LASTLINE_END) {
            pos = LASTLINE_BEG;
            cga_scroll_back();
        }
    }

    if (0 > pos || pos > LASTLINE_END)   hlt();
    cga_cursor_set(pos);
}

/**
 * @brief 输出一个字符串
 * @param str 字符串
 * @param len 字符串长度
 */
void
cga_putstr(const char *str, uint32_t len) {
    wait(cga_get_spinlock());
    for (uint32_t i = 0; i < len; ++i)    cga_putc(str[i], *(cga_get_attribute()));
    signal(cga_get_spinlock());
}
