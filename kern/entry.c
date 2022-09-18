#include "disp.h"

int entry(void) {
    /*uint16_t *pv = (uint16_t *)0xb8000;
    pv += 1919;
    *pv = 0xc63;
    pv--;
    *pv = 0x963;
    while (1);*/
    /*char str[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z' };

    //kprint_str("welcome kernel!\n");
    for (size_t i = 0; i < VGA_HIGH - 1; ++i) {
        char ch = str[i];
        for (size_t j = 0; j < VGA_WIDTH - i - 1; ++j)
            kprint_char(ch);
        kprint_char('\n');
    }
    kprint_str("hello world!\n");
    kprint_char('\b');*/
    init_disp();
    kprint_str("a\nb\t\b");

    while (1);
    // should not return
    return 0;
}