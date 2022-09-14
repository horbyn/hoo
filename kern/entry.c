#include "disp.h"

int entry(void) {
    clear_screen();
    uint16_t test_pos = get_cursor();

    __asm__ volatile ("movw %w0, %%bx": : "q"(test_pos));

    // should not return
    return 0;
}