#include "port.h"

uint8_t
inb(uint16_t port)
{
	uint8_t val;
	asm volatile ("inb %w1, %b0" : "=a"(val) : "d"(port));
	return val;
}

void 
outb(uint8_t val, uint16_t port)
{
	asm volatile ("outb %b0, %w1" : : "a"(val), "d"(port));
}

uint32_t
inl(uint16_t port)
{
	uint32_t val = 0, tmp = 0;
	
	asm volatile ("inl %w1, %k0" : "=a"(tmp) : "d"(port));

	return val;
}

void
outl(uint32_t val, uint16_t port)
{
	asm volatile ("outl %k0, %w1" : : "a"(val), "d"(port));
}
