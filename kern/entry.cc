#include "kernel.hpp"

// C++ will mangle the function name because of its overloading
// But we specify the kernel binary entry is the function name
// `entry` in `kernel.ld`
// So we must tell the compiler not to modify our function name
// by `extern C`
extern "C" void
entry(void) {
    using namespace hoo;

    Kernel kern;
    kern.execute();

    // MUST stop here!
    __asm__ ("hlt");
}
