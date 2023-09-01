/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "cga.h"

static char __buff_cga[CGA_WIDTH * CGA_HIGH];
static char __arr_dig[32];

/**
 * @brief clear screen
 */
void
clear_screen(void) {
    set_cursor(0, 0);

    bzero(__buff_cga, sizeof(__buff_cga));
    cga_putstr(__buff_cga, sizeof(__buff_cga));

    set_cursor(0, 0);
}

/**
 * @brief Set the cursor
 * 
 * @param row row
 * @param col column
 */
void
set_cursor(int row, int col) {
    // 0x3b0-0x3bf MDA
    // 0x3c0-0x3cf CGA
    // 0x3d0-0x3df EGA/VGA

    // CRT:
    // addr = 0x3d4; data = 0x3d5
    // CRT Data
    // 0xe: cursor location high reg
    // 0xf: cursor location low reg

    uint16_t pos = ROWCOL_TO_POS(row, col);

    uint8_t low = pos & 0xff;
    uint16_t hig = pos & 0xff00;
    hig >>= 8;
    // set low 8-bit
    outb(0xf, CRT_CTL);
    outb(low, CRT_CTL + 1);

    // set high 8-bit
    outb(0xe, CRT_CTL);
    outb((uint8_t)hig, CRT_CTL + 1);
}

/**
 * @brief Get the cursor
 * 
 * @return pos the index about the memory that regard as a array
 */
uint16_t
get_cursor(void) {
    uint8_t low;
    uint16_t hig;
    outb(0xf, CRT_CTL);                                     // low
    low = inb(CRT_CTL + 1);
    outb(0xe, CRT_CTL);                                     // high
    hig = inb(CRT_CTL + 1);

    return ((hig <<= 8) | low);
}

/**
 * @brief scroll back a line
 * 
 */
void
scroll_back() {
    // scroll all lines but first line
    for (size_t i = 1; i <= CGA_HIGH - 1; ++i) {
        for (int j = 0; j < CGA_WIDTH; ++j) {
            uint32_t from = VIDEO_MEM + (i * CGA_WIDTH + j) * 2;
            uint32_t to = VIDEO_MEM + ((i - 1) * CGA_WIDTH + j) * 2;
            uint16_t *pfrom = (uint16_t *)from;
            uint16_t *pto = (uint16_t *)to;

            *pto = *pfrom;
        }

        // last one fill in space
        if (i == CGA_HIGH - 1) {
            uint16_t *pv = (uint16_t *)VIDEO_MEM;
            pv += (CGA_HIGH - 1) * CGA_WIDTH;
            for (size_t j = 0; j < CGA_WIDTH; ++j, ++pv)
                *pv = 0xf20;
        }
    }

}

/**
 * @brief print a character
 * @param ch character to be printed
 */
void
cga_putc(char ch) {

    uint16_t *pv = (uint16_t *)VIDEO_MEM;
    uint16_t pos = get_cursor();
    int row = POS_TO_ROW(pos), col = POS_TO_COL(pos);

    // text mode
    uint16_t tch = 0;
    tch |= 0xf00;

    // special: `\b` `\t` `\n`
    if (ch == '\b') {
        if (!(row == 0 && col == 0)) {
            do {
                pos--;

                // new v.m. addr
                pv = (uint16_t *)VIDEO_MEM;
                pv += pos;
                tch |= ' ';
                *pv = tch;

                if (col - 1 < 0) {
                    col = CGA_WIDTH - 1;
                    row--;
                } else    col -= 1;
            } while ((*(pv - 1) | 0xff20) == 0xff20);
        }
    } else if (ch == '\t') {
        // mod 4
        int spaces = SIZE_TAG - (pos % SIZE_TAG);
        tch |= ' ';

        // fill with spaces
        pv += pos;
        for (size_t i = 0; i < spaces; ++i, ++pv)
            *pv = tch;
        
        // update col, row
        if (col + spaces >= CGA_WIDTH) {
            row++;
            col = 0;
        } else    col += spaces;
    } else if (ch == '\n') {
        // only affect cursor
        if (row <= CGA_HIGH - 2)    row++;
        else    scroll_back();

        col = 0;
    } else {
        pv += pos;
        tch |= ch;
        *pv = tch;

        if (col + 1 >= CGA_WIDTH) {
            col = 0;
            if (row <= CGA_HIGH - 2)    row++;
            else    scroll_back();
        } else    col += 1;
    }

    set_cursor(row, col);

}

/**
 * @brief print a string
 * @param str string
 * @param strlen string size
 */
void
cga_putstr(const char *str, size_t strlen) {

    for (size_t i = 0; i < strlen; ++i)
        cga_putc(str[i]);
}

/**
 * @brief kernel print int
 * @param dig decimal digit
 */
void
cga_putint(int dig) {
    if (dig == 0) {
        cga_putc('0');
        return;
    }

    memset(__arr_dig, 0, sizeof(__arr_dig));

    int i = 0;
    while (dig) {
        int remaider = dig % 10;
        __arr_dig[i] = ('0' + remaider);
        dig /= 10;
        ++i;
    }

    while (i)
        cga_putc(__arr_dig[--i]);
}

/**
 * @brief kernel print hex
 * @param dig hex digit
 */
void
cga_puthex(uint32_t dig) {
    if (dig == 0) {
        cga_putc('0');
        return;
    }

    memset(__arr_dig, 0, sizeof(__arr_dig));

    int i = 0;
    while (dig) {
        int remaider = dig % 16;
        if (remaider <= 9)
            __arr_dig[i] = (char)('0' + remaider);
        else
            __arr_dig[i] = (char)(87 + remaider);
        dig /= 16;
        ++i;
    }

    cga_putstr("0x", 2);
    while (i)
        cga_putc(__arr_dig[--i]);
}
