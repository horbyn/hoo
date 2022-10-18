#include "disp.h"
#include "inte.h"
#include "mm.h"

int
entry(void) {
    init_disp();
    init_pic();
    init_interrupt();

    //////////// test idt //////////////
    //int a = 3, b = 0;
    //int c = a / b;
    get_phymm();

    // __asm__ volatile ("sti");    // open timer
    __asm__ ("hlt");
    // should not return
    return 0;
}