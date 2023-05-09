#pragma once
#ifndef __KERN_DESCRIPTOR_ACCESSBYTES_HPP__
#define __KERN_DESCRIPTOR_ACCESSBYTES_HPP__

#include "type.hpp"

namespace hoo {

/**
 * @brief access bytes definition
 */
class AccessBytes {
    /*
     * `this->union_` leaves for the deriver to handle
     *
     * used for GDT             used for OTHER
     * ┌─┬──┬─┬─┬─┬──┬──┬─┐     ┌─┬──┬─┬─┬─┬─┬─┬─┐
     * │7│ 6│5│4│3│ 2│ 1│0│     │7│ 6│5│4│3│2│1│0│
     * ├─┼──┴─┼─┼─┼──┼──┼─┤     ├─┼──┴─┼─┼─┴─┴─┴─┤
     * │P│DPL │S│E│DC│RW│A│     │P│DPL │S│   Type│
     * └─┴────┴─┴─┴──┴──┴─┘     └─┴────┴─┴───────┘
     */
protected:

    uint8_t union_ :4;
    uint8_t sys_   :1;                                      // sys segment: set if it is
    uint8_t dpl_   :2;                                      // privilege
    uint8_t ps_    :1;                                      // present: set if in memory

public:
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

    /**
     * @brief Get the sys field
     * 
     * @return 0 or 1
     */
    uint8_t get_sys() const;
    /**
     * @brief Get the dpl field
     * 
     * @return 0, 1, 2 or 3
     */
    uint8_t get_dpl() const;
    /**
     * @brief Get the ps field
     * 
     * @return 0 or 1
     */
    uint8_t get_ps() const;

    ////////////////////////////////////////////////////////
    /// Now i think SETTER is useless because the access ///
    /// bytes is only used in descriptor constructing;   ///
    /// So i will always used it temporary               ///
    ////////////////////////////////////////////////////////
};

/**
 * @brief access bytes for gdt
 */
class AcsGdt: public AccessBytes {
public:

    AcsGdt() = default;
    ~AcsGdt() = default;
    /**
     * @brief Construct a new Acs Gdt object
     * 
     * @param acs best left clear
     * @param rw readable or writable
     * @param dc direction or conforming
     * @param exe executable
     * @param sys is it a system segment?
     * @param dpl privilege
     * @param ps present
     */
    AcsGdt(uint8_t acs, uint8_t rw, uint8_t dc, uint8_t exe,
        uint8_t sys, uint8_t dpl, uint8_t ps);

    /**
     * @brief Get the acs field
     * 
     * @return 0 or 1
     */
    uint8_t get_acs() const;
    /**
     * @brief Get the rw field
     * 
     * @return 0 or 1
     */
    uint8_t get_rw() const;
    /**
     * @brief Get the dc field
     * 
     * @return 0 or 1
     */
    uint8_t get_dc() const;
    /**
     * @brief Get the exe field
     * 
     * @return 0 or 1
     */
    uint8_t get_exe() const;
};

}

#endif
