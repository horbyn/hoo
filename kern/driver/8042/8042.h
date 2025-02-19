#pragma once
#ifndef __KERN_DRIVER_8042_8042_H__
#define __KERN_DRIVER_8042_8042_H__

#include "cmd_8042.h"
#include "kern/utilities/circular_buffer.h"

#define MAXSIZE_KBBUFF  1024

cclbuff_t *get_kb_buff(void);
void      init_8042(void);
void      ps2_intr(void);

#endif
