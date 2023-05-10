#include "comm.hpp"

namespace hoo {

constexpr size_t SIZE_GDT = 8;
static Gdt __gdt[SIZE_GDT]{};

void
setup_gdt() {
    __gdt[0] = Gdt::get_gdt(0, 0, {}, {});                  // #0 null
    __gdt[1] = Gdt::get_gdt(0xfffff, 0,
        AcsGdt{0, 1, 0, 1, 1, 0, 1}, Flags{0, 0, 1, 1});    // #1 kernel code
    __gdt[2] = Gdt::get_gdt(0xfffff, 0,
        AcsGdt{0, 1, 0, 0, 1, 0, 1}, Flags{0, 0, 1, 1});    // #2 kernel data
    __gdt[3] = Gdt::get_gdt(0xfffff, 0,
        AcsGdt{0, 1, 0, 1, 1, 3, 1}, Flags{0, 0, 1, 1});    // #3 user code
    __gdt[4] = Gdt::get_gdt(0xfffff, 0,
        AcsGdt{0, 1, 0, 0, 1, 3, 1}, Flags{0, 0, 1, 1});    // #4 user data
}

void
init_gdt() {
    GdtRegister gdtr{NELEMS(__gdt) - 1, __gdt};
    enable_gdt(&gdtr);
}

} // end namespace monitor
