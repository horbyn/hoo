#pragma once
#ifndef __KERN_COMM_HPP__
#define __KERN_COMM_HPP__

#include "descriptor/Gdt.hpp"

namespace hoo {

void setup_gdt();
void init_gdt();

} // end namespace hoo

#endif
