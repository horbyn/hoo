#pragma once
#ifndef __KERN_DESCRIPTOR_GDT_HPP__
#define __KERN_DESCRIPTOR_GDT_HPP__

#include "type.hpp"
#include "AccessBytes.hpp"
#include "Flags.hpp"

namespace hoo {

/**
 * @brief gdt definition
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

    AcsGdt   access_bytes_;
    uint8_t  limit_19_16_     :4;
    /*
     * ┌─┬──┬─┬────────┐
     * │3│ 2│1│       0│
     * ├─┼──┼─┼────────┤
     * │G│DB│L│Reserved│
     * └─┴──┴─┴────────┘
     */

    Flags    flags_;
    uint8_t  base_31_24_;

public:
    Gdt(): limit_15_0_(0), base_15_0_(0), base_23_16_(0),
        access_bytes_({}), limit_19_16_(0), flags_({}),
        base_31_24_(0) {}
    ~Gdt() {}
    /**
     * @brief Construct a new Gdt object
     * 
     * @param limit the maximun of addressing, which units are either 1B or 4KB
     * @param base segment base linear address
     * @param ag the access bytes
     * @param flags flags
     */
    Gdt(uint32_t limit, uint32_t base, const AcsGdt &ag,
        const Flags &flags)
        : access_bytes_(ag), flags_(flags) {
        this->limit_15_0_  = (uint16_t)limit;
        this->limit_19_16_ = (uint16_t)(limit >> 16) & 0xf;
        this->base_15_0_   = (uint16_t)base;
        this->base_23_16_  = (uint8_t)(base >> 16) & 0xff;
        this->base_31_24_  = (uint8_t)(base >> 24) & 0xff;
    }
};

/**
 * @brief definition of GDTR
 */
class GdtRegister {
protected:
    uint16_t size_;                                         // gdt size
    Gdt *linear_;                                           // gdt linear base

public:
    GdtRegister(): size_(0), linear_(nullptr) {}
    ~GdtRegister() {}
    /**
     * @brief Construct a new Gdt Register object
     * 
     * @param sz gdt size
     * @param addr gdt linear addr
     */
    GdtRegister(uint16_t sz, Gdt *addr)
        : size_(sz), linear_(addr) {}
};

} // end namespace hoo

#endif
