#include "HdlGdt.hpp"

namespace hoo {

void
HdlGdt::set_gdt(size_t idx, uint32_t limit, uint32_t base,
const AcsGdt &ag, const Flags &f) {
    if (idx >= SIZE_GDT)    return;

    this->gdt_[idx] = Gdt{limit, base, ag, f};
}

void
HdlGdt::set_gdtr() {
    this->gdtr_ = GdtRegister{NELEMS(this->gdt_) - 1, this->gdt_};
    
    /* fast A20 gate */
    __asm__ (".code16;              \
        inb $0x92, %al;             \
        orb $2, %al;                \
        outb %al, $0x92");

    __asm__ ("lgdt %0" : :"m"(this->gdtr_));

    /* set CR0.PE */
    __asm__ ("movl %cr0, %eax;      \
        orl $1, %eax;               \
        movl %eax, %cr0");

    /* refresh pipeline */
    __asm__ ("ljmp $0x08, $pm_go;   \
    pm_go:;                         \
        .code32;                    \
        movw $0x10,   %ax;          \
        movw %ax,     %ds;          \
        movw %ax,     %es;          \
        movw %ax,     %fs;          \
        movw %ax,     %gs;          \
        movw %ax,     %ss;          \
        movl $0x8000, %esp");                               // stack here MUST be equal of
                                                            // `boot/kern_will_use.inc/SEG_KSTACK`
}

} // end namespace hoo
