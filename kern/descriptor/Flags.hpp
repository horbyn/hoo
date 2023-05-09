#pragma once
#ifndef __KERN_DESCRIPTOR_FLAGS_HPP__
#define __KERN_DESCRIPTOR_FLAGS_HPP__

#include "type.hpp"

namespace hoo {

class Flags {
public:
    uint8_t rsv_  : 1;                                      // reserved
    uint8_t long_ : 1;                                      // long mode if set
    uint8_t db_   : 1;                                      // protected mode
    uint8_t g_    : 1;                                      // granularity

    Flags() = default;
    ~Flags() = default;
    /**
     * @brief Construct a new Flags object
     * 
     * @param rsv reserved filed
     * @param l long mode if set
     * @param db protected mode: set if 32 bit pm.; clear if 16 bit
     * @param g granularity: set if 4KB units; clear if 1B
     */
    Flags(uint8_t rsv, uint8_t l, uint8_t db, uint8_t g);

    /**
     * @brief Get the rsv field
     * 
     * @return 0 or 1
     */
    uint8_t get_rsv() const;
    /**
     * @brief Get the long field
     * 
     * @return 0 or 1
     */
    uint8_t get_long() const;
    /**
     * @brief Get the db field
     * 
     * @return 0 or 1
     */
    uint8_t get_db() const;
    /**
     * @brief Get the g field
     * 
     * @return 0 or 1
     */
    uint8_t get_g() const;
};

}

#endif
