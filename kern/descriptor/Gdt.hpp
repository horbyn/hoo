#pragma once
#ifndef __KERN_DESCRIPTOR_GDT_HPP__
#define __KERN_DESCRIPTOR_GDT_HPP__

#include "type.hpp"
#include "AccessBytes.hpp"
#include "Flags.hpp"

namespace hoo {

/**
 * @brief handle something on setting gdt
 */
class Gdt {
protected:
    uint16_t limit_15_0_;
    uint16_t base_15_0_;
    uint8_t  base_23_16_;
    /*
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤
     * │P│DPL │S│E│DC│RW│A│
     * └─┴────┴─┴─┴──┴──┴─┘
     */

    uint8_t  access_byte_a_   :1;
    uint8_t  access_byte_rw_  :1;
    uint8_t  access_byte_dc_  :1;
    uint8_t  access_byte_e_   :1;
    uint8_t  access_byte_s_   :1;
    uint8_t  access_byte_dpl_ :2;
    uint8_t  access_byte_p_   :1;
    uint8_t  limit_19_16_     :4;
    /*
     * ┌─┬──┬─┬────────┐
     * │3│ 2│1│       0│
     * ├─┼──┼─┼────────┤
     * │G│DB│L│Reserved│
     * └─┴──┴─┴────────┘
     */

    uint8_t  flags_rsv_       :1;
    uint8_t  flags_l_         :1;
    uint8_t  flags_db_        :1;
    uint8_t  flags_g_         :1;
    uint8_t  base_31_24_;

public:
    Gdt() = default;
    ~Gdt() = default;

    /**
     * @brief Get the gdt object
     * 
     * @param limit the maximun of addressing, which units are either 1B or 4KB
     * @param base segment base linear address
     * @param ab the access bytes
     * @param flags flags
     * @return Gdt descriptor
     */
    static Gdt get_gdt(uint32_t limit, uint32_t base, const
        AcsGdt &ab, const Flags &flags);
};

/**
 * @brief definition of GDTR
 */
class GdtRegister {
protected:
    uint16_t size_;                                         // gdt size
    Gdt *linear_;                                           // gdt linear base

public:
    GdtRegister() = default;
    ~GdtRegister() = default;
    /**
     * @brief Construct a new Gdt Register object
     * 
     * @param sz gdt size
     * @param addr gdt linear addr
     */
    GdtRegister(uint16_t sz, Gdt *addr)
        : size_(sz), linear_(addr) {}
};

} // end namespace end

extern void enable_gdt(hoo::GdtRegister *gdt);

#endif
