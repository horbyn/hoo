#include "disp.h"
#include "inte.h"
#include "mm.h"

extern uint8_t __kern_base[], __kern_end[];

int
entry(void) {
    init_disp();
    init_pic();
    init_interrupt();

    kprintf("kern base = %x\nkern end = %x\n\n",
        (uint32_t)__kern_base, (uint32_t)__kern_end);

    init_phymm();

    // __asm__ volatile ("sti");    // open timer
    __asm__ ("hlt");
    // should not return
    return 0;
}