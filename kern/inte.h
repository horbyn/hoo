#ifndef __KERN_INTE_H__
#define __KERN_INTE_H__

#include "types.h"
#include "x86.h"

#define PIC_MAS_EVEN  0x20
#define PIC_SLA_EVEN  0xa0

void init_pic(void);

#endif
