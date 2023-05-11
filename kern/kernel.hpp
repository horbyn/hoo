#pragma once
#ifndef __KERN_KERNEL_HPP__
#define __KERN_KERNEL_HPP__

#include "module/ProtectedMode.hpp"

namespace hoo {

/**
 * @brief definition of kernel
 */
class Kernel final {
private:
    ProtectedMode pm_;                                      // operation of entering pm.

public:
    Kernel();
    ~Kernel();

    /**
     * @brief kernel executing
     */
    void execute();
};

} // end namespace hoo

#endif
