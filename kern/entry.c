#include "disp.h"
#include "inte.h"

int
entry(void) {
    init_disp();
    init_pic();
    init_interrupt();

    //////////// test idt //////////////
    //int a = 3, b = 0;
    //int c = a / b;

    __asm__ volatile ("sti");
    while (1);
    // should not return
    return 0;
}