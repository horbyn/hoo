#include "inte.h"

// IDT
__attribute__((aligned(0x8)))
static idt_entry_t idt[IDT_ENTRIES_NUM];

// IDTR
static idtr_t idtr;

void
init_pic(void) {
    // IRQ0-15 --> vec.0x20-0x2f

    // =========== master ============
    // ICW1
    // 7  6 5 4  3    2   1    0
    // ┌─┼─┼─┼─┼────┼───┼────┼───┐
    // │0│0│0│1│LTIM│ADI│SNGL│IC4│
    // └─┼─┼─┼─┼────┼───┼────┼───┘
    // 0  0 0 1  0    0   0    1
    outb(0x11, PIC_MAS_EVEN);
    // ICW2
    //  7  6   5  4  3  2   1   0
    // ┌──┼──┼──┼──┼──┼───┼───┼───┐
    // │T7│T6│T5│T4│T3│ID2│ID1│ID0│
    // └──┼──┼──┼──┼──┼───┼───┼───┘
    //  0  0   1  0  0  0   0   0
    outb(0x20, PIC_MAS_EVEN + 1);
    // ICW3
    //  7  6   5  4  3  2  1  0
    // ┌──┼──┼──┼──┼──┼──┼──┼──┐
    // │S7│S6│S5│S4│S3│S2│S1│S0│
    // └──┼──┼──┼──┼──┼──┼──┼──┘
    //  0  0   0  0  0  1  0  0
    outb(4, PIC_MAS_EVEN + 1);
    // ICW4
    //  7 6 5   4   3   2   1    0
    // ┌─┼─┼─┼────┼───┼───┼────┼───┐
    // │0│0│0│SFNM│BUF│M/S│AEOI│uPM│
    // └─┼─┼─┼────┼───┼───┼────┼───┘
    //  0 0 0   0   0   0   1    1
    outb(3, PIC_MAS_EVEN + 1);

    // ============ slave ============
    // ICW1
    outb(0x11, PIC_SLA_EVEN);
    // ICW2
    outb(0x28, PIC_SLA_EVEN + 1);
    // ICW3
    //  7 6 5 4 3  2   1   0
    // ┌─┼─┼─┼─┼─┼───┼───┼───┐
    // │0│0│0│0│0│ID2│ID1│ID0│
    // └─┼─┼─┼─┼─┼───┼───┼───┘
    //  0 0 0 0 0  0   1   0
    outb(2, PIC_SLA_EVEN + 1);
    // ICW4
    outb(3, PIC_SLA_EVEN + 1);
}

void
init_idt(void) {
    // load idtr
    idtr.limit = sizeof(idt) - 1;
    idtr.base = idt;
    uint32_t idtr_addr = &idtr;
    __asm__ volatile ("lidt %k0" : : "a"(idtr_addr));
}
