#include "disp.h"

void
clear_screen(void) {
    uint8_t *p = (uint8_t *)VIDEO_MEM;

    // VGA text mode
    for (int i = 0; i < 25 * 80; ++i) {
        *p++ = ' ';
        *p++ = 0x0f;    // bg: black and fg: white
    }
}
