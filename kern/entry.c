#include "disp.h"
#include "inte.h"
#include "mm.h"

extern uint8_t __kern_base[], __kern_end[];

int
entry(void) {
    //init_disp();
    //init_pic();
    //init_interrupt();
    kprintf("abcd%defgh", 100001);

    //////////// test idt //////////////
    //int a = 3, b = 0;
    //int c = a / b;
    //kprint_str("kern base = ");
    //kprint_hex((uint32_t)__kern_base);
    //kprint_str("\nkern end = ");
    //kprint_hex((uint32_t)__kern_end);
    //kprint_str("\n\n");

    //init_phymm();

    // __asm__ volatile ("sti");    // open timer
    __asm__ ("hlt");
    // should not return
    return 0;
}