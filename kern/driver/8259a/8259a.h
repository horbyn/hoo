#pragma once
#ifndef __KERN_DRIVER_8259A_8259A_H__
#define __KERN_DRIVER_8259A_8259A_H__

#include "8259a_stuff.h"
#include "kern/x86.h"

void set_icw1(uint8_t cmd);
void set_icw2(uint8_t master, uint8_t slave);
void set_icw3(uint8_t irq_pin);
void set_icw4(uint8_t cmd);
void disable_mask_ocw1(uint8_t irq_pin);
void enable_mask_ocw1(uint8_t irq_pin);

#endif
