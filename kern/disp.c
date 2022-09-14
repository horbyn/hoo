#include "disp.h"

void
init_disp(void) {
    // s1. clear screen
    clear_screen();

    // s2. set cursor to 0
    set_cursor(0);
}

void
clear_screen(void) {
    uint8_t *p = (uint8_t *)VIDEO_MEM;

    // VGA text mode
    for (int i = 0; i < 25 * 80; ++i) {
        *p++ = ' ';
        *p++ = 0x0f;    // bg: black and fg: white
    }
}

void
set_cursor(uint16_t pos) {
    // CRT:
    // addr = 0x3d4; data = 0x3d5
    // CRT Data
    // 0xe: cursor location high reg
    // 0xf: cursor location low reg

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

    return ((hig <<= 8) || low);
}

void
kprint_char(char ch) {
    //uint16_t *pv = (uint16_t *)VIDEO_MEM;
}
