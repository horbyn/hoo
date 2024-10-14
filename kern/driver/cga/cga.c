/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "cga.h"

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
 * @param row row
 * @param col column
 */
static void
cga_cursor_set(int row, int col) {
    uint16_t pos = TO_POS(row, col);

    uint8_t low = pos & 0xff;
    uint16_t hig = pos & 0xff00;
    hig >>= 8;
    // set low 8-bit
    outb(CGA_CURSOR_LOW, CGA_REG_INDEX);
    outb(low, CGA_REG_DATA);

    // set high 8-bit
    outb(CGA_CURSOR_HIGH, CGA_REG_INDEX);
    outb((uint8_t)hig, CGA_REG_DATA);
}

/**
 * @brief print a character
 * @param ch   character to be printed
 * @param attr attribute
 */
void
cga_putc(char ch, uint8_t attr) {
    uint16_t *vm = (uint16_t*)VIDEO_MEM;
    uint16_t ch_attr = (uint16_t)attr << 8;
    uint16_t pos = cga_cursor_get();

    // special: `\b` `\t` `\n`
    if (ch == '\b') {
        if (!(TO_ROW(pos) == 0 && TO_COL(pos) == 0)) {
            *(vm + (--pos)) = (ch_attr | ' ');
        }
    } else if (ch == '\t') {
        // mod 4
        int spaces = SIZE_TAG - (pos % SIZE_TAG);
        ch_attr |= ' ';

        // fill with spaces
        for (uint32_t i = 0; i < spaces; ++i, ++pos) {
            *(vm + pos) = ch_attr;
        }
    } else if (ch == '\n') {
        uint16_t left = TO_POS(CGA_HIGH - 1, 0),
            right = TO_POS(CGA_HIGH - 1, CGA_WIDTH - 1);
        if (left <= pos && pos <= right) {
            pos = left;
        } else {
            pos = TO_POS(TO_ROW(pos), 0);
        }
    } else {
        *(vm + pos++) = (ch_attr | ch);
    }

    cga_cursor_set(TO_ROW(pos), TO_COL(pos));
}
