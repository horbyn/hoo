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
}

} // end namespace hoo
