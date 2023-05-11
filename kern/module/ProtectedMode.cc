#include "ProtectedMode.hpp"

namespace hoo {

void
ProtectedMode::enable_pm() {
    this->hg_.set_gdt(0, 0, 0, {}, {});                     // #0 null
    this->hg_.set_gdt(1, 0xfffff, 0,
        AcsGdt{0, 1, 0, 1, 1, 0, 1}, Flags{0, 0, 1, 1});    // #1 kernel code
    this->hg_.set_gdt(2, 0xfffff, 0,
        AcsGdt{0, 1, 0, 0, 1, 0, 1}, Flags{0, 0, 1, 1});    // #2 kernel data
    this->hg_.set_gdt(3, 0xfffff, 0,
        AcsGdt{0, 1, 0, 1, 1, 3, 1}, Flags{0, 0, 1, 1});    // #3 user code
    this->hg_.set_gdt(4, 0xfffff, 0,
        AcsGdt{0, 1, 0, 0, 1, 3, 1}, Flags{0, 0, 1, 1});    // #4 user data

    this->hg_.set_gdtr();
}

} // end namespace hoo
