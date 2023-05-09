#include "Flags.hpp"

namespace hoo {

Flags::Flags(uint8_t rsv, uint8_t l, uint8_t db, uint8_t g)
: rsv_(rsv), long_(l), db_(db), g_(g) {}

uint8_t
Flags::get_rsv() const {
    return this->rsv_ == 0? 0: 1;
}

uint8_t
Flags::get_long() const {
    return this->long_ == 0? 0: 1;
}

uint8_t
Flags::get_db() const {
    return this->db_ == 0? 0: 1;
}

uint8_t
Flags::get_g() const {
    return this->g_ == 0? 0: 1;
}

}
