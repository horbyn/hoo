#ifndef __INC_PORT_H__
#define __INC_PORT_H__

#include "types.h"

void inb(uint8_t *mem, uint8_t port);
void outb(uint8_t port, uint8_t *mem);
void inl(uint32_t *mem, uint8_t port);
void outl(uint32_t port, uint32_t *mem);

#endif
