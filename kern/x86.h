#ifndef __KERN_X86_H__
#define __KERN_X86_H__

#include "types.h"

#define SIZE_TAG 4

static inline void
cli() {
    __asm__ ("cli\n\t");
}

static inline void
sti() {
    __asm__ ("sti\n\t");
}

// fetch data from the specified port
static inline uint8_t
inb(uint16_t port) {
    uint8_t val;
    // %al --> val
    // port --> %dx
    // is equal of `inb %dx, %al` that
    // the data is from %dx to %al
    __asm__ volatile ("inb %w1, %b0" : "=a"(val) : "d"(port));
    return val;
}

// data write to the specified port
static inline void
outb(uint8_t val, uint16_t port) {
    // val --> %al
    // port --> %dx
    // data from %al is transported to port %dx pointed to
    __asm__ volatile ("outb %b0, %w1" : : "a"(val), "d"(port));
}

// fetch data from the specified port
static inline uint32_t
inl(uint16_t port) {
    uint32_t val = 0;
    __asm__ volatile ("inl %w1, %k0" : "=a"(val) : "d"(port));
    return val;
}

// data write to the specified port
static inline void
outl(uint32_t val, uint16_t port) {
    __asm__ volatile ("outl %k0, %w1" : : "a"(val), "d"(port));
}


#endif
