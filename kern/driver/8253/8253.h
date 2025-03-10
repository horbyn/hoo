#pragma once
#ifndef __KERN_DRIVER_8253_8253_H__
#define __KERN_DRIVER_8253_8253_H__

#include "8253_stuff.h"
#include "kern/x86.h"

void set_command(sc_t sc, am_t am, om_t om, bm_t bm);
void set_counter(uint32_t frequency);

#endif
