#pragma once
#ifndef __KERN_DESCRIPTOR_ACCESSBYTES_HPP__
#define __KERN_DESCRIPTOR_ACCESSBYTES_HPP__

#include "type.hpp"

namespace hoo {

/**
 * @brief access bytes definition
 */
class AccessBytes {
    uint8_t union_ :4;
    uint8_t sys_   :1;                                      // sys segment: set if it is
    uint8_t dpl_   :2;                                      // privilege
    uint8_t ps_    :1;                                      // present: set if in memory

    AccessBytes() = default;
    ~AccessBytes() = default;
    /**
     * @brief Constructor
     * 
     * @param un union feature
     * @param sys sys segment: set if it is
     * @param dpl privilege
     * @param ps present: set if in memory
     */
    AccessBytes(uint8_t un, uint8_t sys, uint8_t dpl,
        uint8_t ps)
        : union_(un), sys_(sys), dpl_(dpl), ps_(ps) {}
};


/**
 * @brief access bytes for gdt
 */
class AcsGdt: public AccessBytes {
public:
    /*
     * used for GDT             used for OTHER
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐     ┌─┬──┬─┬─┬─┬─┬─┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│     │7│ 6│5│4│3│2│1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤     ├─┼──┴─┼─┼─┴─┴─┴─┤
     * │P│DPL │S│E│DC│RW│A│     │P│DPL │S│   Type│
     * └─┴────┴─┴─┴──┴──┴─┘     └─┴────┴─┴───────┘
     */

    union un_t{
        struct gdt_t{
            uint8_t acs_ :1;                                // accessed: best left clear
            uint8_t rw_  :1;                                // readable or writable
            uint8_t dc_  :1;                                // direction or conforming: I DON KNOWN
            uint8_t exe_ :1;                                // execute: set if executable
        } gdt_;                                             // used for GDT
        uint8_t type_    :4;                                // used for TSS/LDT
    } un_;

    AccessBytes() = default;
    ~AccessBytes() = default;
    /**
     * @brief Constructor
     * 
     * @param acs accessed: best left clear
     * @param rw readable or writable
     * @param dc direction or conforming(different behavior
     * between code and data segments)
     * @param exe execute: set if executable
     * @param sys sys segment: set if it is
     * @param dpl privilege
     * @param ps present: set if in memory
     */
    AccessBytes(uint8_t acs, uint8_t rw, uint8_t dc,
        uint8_t exe, uint8_t sys, uint8_t dpl, uint8_t ps)
        : un_.gdt_.acs_(asc), un_.gdt_.rw_(rw), 
        un_.gdt_.dc_(dc), un_.gdt_.exe_(exe), sys_(sys),
        dpl_(dpl), ps_(ps) {}
};

}

#endif
