#include "Gdt.hpp"

namespace hoo {

Gdt
Gdt::get_gdt(uint32_t limit, uint32_t base, const AcsGdt &ab,
const Flags &flags) {
    Gdt temp{};

    temp.limit_15_0_      = (uint16_t)limit;
    temp.limit_19_16_     = (uint16_t)(limit >> 16) & 0xf;
    temp.base_15_0_       = (uint16_t)base;
    temp.base_23_16_      = (uint8_t)(base >> 16) & 0xff;
    temp.base_31_24_      = (uint8_t)(base >> 24) & 0xff;
    temp.access_byte_a_   = ab.get_acs();
    temp.access_byte_rw_  = ab.get_rw();
    temp.access_byte_dc_  = ab.get_dc();
    temp.access_byte_e_   = ab.get_exe();
    temp.access_byte_s_   = ab.get_sys();
    temp.access_byte_dpl_ = ab.get_dpl();
    temp.access_byte_p_   = ab.get_ps();
    temp.flags_l_         = flags.get_long();
    temp.flags_db_        = flags.get_db();
    temp.flags_g_         = flags.get_g();

    return temp;
}

}
