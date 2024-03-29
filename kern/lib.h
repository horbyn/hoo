#ifndef __KERN_LIB_H__
#define __KERN_LIB_H__

#include "types.h"

uint32_t strlen(const char *);

void memset(void *, uint8_t, size_t);
int memmove(void *, const void *, size_t);

#endif
