#pragma once
#ifndef __KERN_MODULE_PROTECTEDMODE_HPP__
#define __KERN_MODULE_PROTECTEDMODE_HPP__

#include "descriptor/HdlGdt.hpp"

namespace hoo {

/**
 * @brief enable pm.
 * 
 */
class ProtectedMode {
protected:
    HdlGdt hg_;

public:
    ProtectedMode(): hg_({}) {}
    ~ProtectedMode() {}

    /**
     * @brief enter protected mode
     */
    void enable_pm();
};

} // end namespace hoo

#endif
