#include "disp.h"

static uint32_t *g_cursor = (uint32_t *)GLOBAL_CURSOR;

void
init_disp(void) {
    // s1. clear screen
    clear_screen();

    // s2. set cursor to 0
    set_cursor(0, 0);
}

void
clear_screen(void) {
    uint8_t *p = (uint8_t *)VIDEO_MEM;

    // VGA text mode
    for (int i = 0; i < VGA_HIGH * VGA_WIDTH; ++i) {
        *p++ = ' ';
        *p++ = 0x0f;    // bg: black and fg: white
    }
}

void
set_cursor(int row, int col) {
    // CRT:
    // addr = 0x3d4; data = 0x3d5
    // CRT Data
    // 0xe: cursor location high reg
    // 0xf: cursor location low reg

    uint16_t pos = row * VGA_WIDTH + col;
    *g_cursor = pos;

    uint8_t low = pos & 0xff;
    uint16_t hig = pos & 0xff00;
    hig >>= 8;
    // set low 8-bit
    outb(0xf, 0x3d4);
    outb(low, 0x3d5);

    // set high 8-bit
    outb(0xe, 0x3d4);
    outb((uint8_t)hig, 0x3d5);
}

uint16_t
get_cursor(void) {
    uint8_t low;
    uint16_t hig;
    outb(0xf, 0x3d4);   // low
    low = inb(0x3d5);
    outb(0xe, 0x3d4);   // high
    hig = inb(0x3d5);

    return ((hig <<= 8) | low);
}

void
kprint_char(char ch) {
    uint16_t *pv = (uint16_t *)VIDEO_MEM;
    uint16_t pos = (uint16_t)*g_cursor;
    int row = pos / VGA_WIDTH,
        col = pos % VGA_WIDTH;

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
                    col = VGA_WIDTH - 1;
                    row--;
                } else    col -= 1;
            } while ((*(pv - 1) | 0xff20) == 0xff20);
        }
    } else if (ch == '\t') {
        if (col != 0) {
            // calc mod 4
            int rem = (pos + 1) % SIZE_TAG,
                rest = rem != 0 ? SIZE_TAG - rem + 1
                    : 1;
            tch |= ' ';

            // complete spaces
            pv += pos;
            for (size_t i = 0; i < rest; ++i, ++pv)
                *pv = tch;
            
            if (col + rest >= VGA_WIDTH) {
                row++;
                col = 0;
            } else    col += rest;
        }
    } else if (ch == '\n') {
        // only affect cursor
        if (row <= 23)    row++;
        else    scroll_back();

        col = 0;
    } else {
        pv += pos;
        tch |= ch;
        *pv = tch;

        if (col + 1 >= VGA_WIDTH) {
            row++;
            col = 0;
        } else    col += 1;
    }

    set_cursor(row, col);
}

void
kprint_str(const char *str) {
    size_t str_len = strlen(str);

    for (size_t i = 0; i < str_len; ++i)
        kprint_char(str[i]);
}

void
kprint_int(int dig) {
    if (dig == 0) {
        kprint_char('0');
        return;
    }

    char arr_dig[32];
    memset(arr_dig, 0, sizeof(arr_dig));

    int i = 0;
    while (dig) {
        int rem = dig % 10;
        arr_dig[i] = ('0' + rem);
        dig /= 10;
        ++i;
    }

    while (i)
        kprint_char(arr_dig[--i]);
}

void
kprint_hex(uint32_t dig) {
    if (dig == 0) {
        kprint_char('0');
        return;
    }

    char arr_dig[32];
    memset(arr_dig, 0, sizeof(arr_dig));

    int i = 0;
    while (dig) {
        int rem = dig % 16;
        if (rem <= 9)
            arr_dig[i] = (char)('0' + rem);
        else
            arr_dig[i] = (char)(87 + rem);
        dig /= 16;
        ++i;
    }

    kprint_str("0x");
    while (i)
        kprint_char(arr_dig[--i]);
}

void
kprintf(const char *format, ...) {
    
}

void
scroll_back(void) {
    // scroll all lines but first line
    for (size_t i = 1; i <= VGA_HIGH - 1; ++i) {
        for (int j = 0; j < VGA_WIDTH; ++j) {
            uint32_t from = VIDEO_MEM + (i * VGA_WIDTH + j) * 2;
            uint32_t to = VIDEO_MEM + ((i - 1) * VGA_WIDTH + j) * 2;
            uint16_t *pfrom = (uint16_t *)from;
            uint16_t *pto = (uint16_t *)to;

            *pto = *pfrom;
        }

        // last one fill in space
        if (i == VGA_HIGH - 1) {
            uint16_t *pv = (uint16_t *)VIDEO_MEM;
            pv += (VGA_HIGH - 1) * VGA_WIDTH;
            for (size_t j = 0; j < VGA_WIDTH; ++j, ++pv)
                *pv = 0xf20;
        }
    }

}
