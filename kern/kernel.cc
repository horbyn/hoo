#include "kernel.hpp"

namespace hoo {

Kernel::Kernel(): pm_({}) {
}

Kernel::~Kernel() {
}

void
Kernel::execute() {
    this->pm_.enable_pm();
}

} // end namespace hoo
