#ifndef __INC_PORT_H__
#define __INC_PORT_H__

#include "types.h"

uint8_t inb(uint16_t);
void outb(uint8_t, uint16_t);
uint32_t inl(uint16_t port);
void outl(uint32_t, uint16_t);

#endif
