#include "port.h"

uint8_t
inb(uint16_t port)
{
	uint8_t val;
	asm volatile ("inb %w1, %b0" : "=a"(val): "d"(port));
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
	return val;
}

void
outl(uint32_t val, uint16_t port)
{
	for (int i = 0; i < 2; ++i)
	{
		asm volatile ("outw %w0, %w1" : : "a"(val), "d"(port));
		val >>= 16;
	}
}
