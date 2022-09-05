#ifndef __KERN_DISP_H__
#define __KERN_DISP_H__

#include "types.h"


#define GLOBAL_CURSOR 0xf000    // global cursor
#define VIDEO_MEM     0xb8000   // video mm. base

void clear_screen(void);

#endif
