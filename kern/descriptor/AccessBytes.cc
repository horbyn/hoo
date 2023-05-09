#include "AccessBytes.hpp"

namespace hoo {

uint8_t
AccessBytes::get_sys() const {
    return this->sys_ == 0 ? 0 : 1;
}

uint8_t
AccessBytes::get_dpl() const {
    return this->dpl_ == 0 ?
        0 : this->dpl_ == 1 ?
            1 : this->dpl_ == 2 ?
                2 : 3;
}

uint8_t
AccessBytes::get_ps() const {
    return this->ps_ == 0 ? 0 : 1;
}

AcsGdt::AcsGdt(uint8_t acs, uint8_t rw, uint8_t dc, uint8_t
exe, uint8_t sys, uint8_t dpl, uint8_t ps)
: AccessBytes(0, sys, dpl, ps) {

    this->union_ =
        (exe <<= 3) | (dc <<= 2) | (rw <<= 1) | acs;
}

uint8_t
AcsGdt::get_acs() const {
    return (this->union_ & 1) == 0 ? 0 : 1;
}

uint8_t
AcsGdt::get_rw() const {
    return (this->union_ & 2) == 0 ? 0 : 1;
}

uint8_t
AcsGdt::get_dc() const {
    return (this->union_ & 4) == 0 ? 0 : 1;
}

uint8_t
AcsGdt::get_exe() const {
    return (this->union_ & 8) == 0 ? 0 : 1;
}


}
