#include "inte.h"

void
init_pic(void) {
    // =========== master ============
    // ICW1
    outb(0x11, PIC_MAS_EVEN);
    // ICW2
    outb(0x20, PIC_MAS_EVEN + 1);
    // ICW3
    outb(4, PIC_MAS_EVEN + 1);
    // ICW4
    outb(3, PIC_MAS_EVEN + 1);

    // ============ slave ============
    // ICW1
    outb(0x11, PIC_SLA_EVEN);
    // ICW2
    outb(0x28, PIC_SLA_EVEN + 1);
    // ICW3
    outb(2, PIC_SLA_EVEN + 1);
    // ICW4
    outb(3, PIC_SLA_EVEN + 1);

    // OCW3

}
