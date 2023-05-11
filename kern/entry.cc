#include "kernel.hpp"

void
entry(void) {
    using namespace hoo;

    Kernel kern;
    kern.execute();

    // MUST stop here!
    __asm__ ("hlt");
}
