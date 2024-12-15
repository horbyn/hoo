/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "cga.h"
#include "user/lib.h"
#include "kern/utilities/spinlock.h"

/**
 * @brief get cga spinlock
 * 
 * @return spinlock 
 */
static spinlock_t *
cga_get_spinlock(void) {
    static spinlock_t cga_spinlock;
    return &cga_spinlock;
}

/**
 * @brief get cga attribute
 * 
 * @return cga attribute
 */
static uint8_t *
cga_get_attribute(void) {
    static uint8_t cga_attr = 0;
    return &cga_attr;
}

/**
 * @brief screen scroll back
 */
static void
cga_scroll_back(void) {
    uint16_t beg = TO_POS(1, 0);
    uint16_t *vm = (uint16_t *)VIDEO_MEM;
    memmove(vm, vm + beg, (LASTLINE_END - beg + 1) * sizeof(uint16_t));

    // fill with space the last line
    for (uint32_t i = LASTLINE_BEG; i <= LASTLINE_END; ++i)
        vm[i] = WHITH_CHAR;
}

/**
 * @brief get the cursor
 * 
 * @return pos the index about the memory that regard as a array
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
 * @brief set the cursor
 * 
 * @param pos video memory position
 */
static void
cga_cursor_set(uint16_t pos) {
    // set low 8-bit
    outb(CGA_CURSOR_LOW, CGA_REG_INDEX);
    outb((uint8_t)(pos & 0xff), CGA_REG_DATA);

    // set high 8-bit
    outb(CGA_CURSOR_HIGH, CGA_REG_INDEX);
    outb((uint8_t)((pos & 0xff00) >> 8), CGA_REG_DATA);
}

/**
 * @brief initialize the cga
 */
void
cga_init(void) {
    spinlock_init(cga_get_spinlock());
}

/**
 * @brief clear screen
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
 * @brief setup cga attribute
 * 
 * @param color  cga color
 * @param bright cga brightness
 */
void
cga_set_attribute(color_t color, bright_t bright) {

    /*
     * The CGA specification as following, only setup the foreground here
     * 
     * Bit 76543210
     *     ||||||||
     *     |||||^^^-fore colour
     *     ||||^----fore colour bright bit
     *     |^^^-----back colour
     *     ^--------back colour bright bit OR enables blinking Text
     */

    *(cga_get_attribute()) = (uint8_t)0 | ((uint8_t)bright << 3) | (uint8_t)color;
}

/**
 * @brief print a character
 * @param ch   character to be printed
 * @param attr attribute
 */
static void
cga_putc(char ch, uint8_t attr) {
    uint16_t *vm = (uint16_t*)VIDEO_MEM;
    uint16_t ch_attr = (uint16_t)attr << 8;
    uint16_t pos = cga_cursor_get();

    // special: `\b` `\t` `\n`
    if (ch == '\b') {
        if (!(TO_ROW(pos) == 0 && TO_COL(pos) == 0)) {
            *(vm + (--pos)) = (ch_attr | ' ');
            uint16_t spaces = 0;
            for (uint16_t i = pos - 1; (vm[i] & 0xff) == ' ' || i == 0; --i)
                ++spaces;
            if (spaces > 1)    pos -= spaces;
        }

    } else if (ch == '\t') {
        // mod 4
        int spaces = SIZE_TAG - (pos % SIZE_TAG);

        // fill with spaces
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
 * @brief print a string
 * @param str  string
 * @param len  string length
 */
void
cga_putstr(const char *str, uint32_t len) {
    wait(cga_get_spinlock());
    for (uint32_t i = 0; i < len; ++i)    cga_putc(str[i], *(cga_get_attribute()));
    signal(cga_get_spinlock());
}
