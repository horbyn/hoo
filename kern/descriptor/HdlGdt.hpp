#pragma once
#ifndef __KERN_DESCRIPTOR_HDLGDT_HPP__
#define __KERN_DESCRIPTOR_HDLGDT_HPP__

#include "Gdt.hpp"
#include "type.hpp"

namespace hoo {

constexpr size_t SIZE_GDT = 8;

/**
 * @brief a class on operating gdt
 */
class HdlGdt {
protected:
    Gdt gdt_[SIZE_GDT];                                     // now i hard code `size_` of 8 simply
    GdtRegister gdtr_;

public:
    HdlGdt() {}
    ~HdlGdt() {}
    /**
     * @brief Set the gdt object
     * 
     * @param idx the table idx(if beyond then nothing happen)
     * @param limit the maximun of addressing, which units are either 1B or 4KB
     * @param base segment base linear address
     * @param ag the access bytes
     * @param f flags
     */
    void set_gdt(size_t idx, uint32_t limit, uint32_t base,
        const AcsGdt &ag, const Flags &f);
    /**
     * @brief Set the gdtr object
     * 
     */
    void set_gdtr();
};

} // end namespace hoo

#endif
