#ifndef __KERN_DISP_H__
#define __KERN_DISP_H__

#include "types.h"
#include "x86.h"

#define GLOBAL_CURSOR 0xf000    // global cursor
#define VIDEO_MEM     0xb8000   // video mm. base

typedef enum tmode_color_t {
    black = 0,
    blue,
    green,
    cyan,
    red,
    magenta,
    brown,
    light_gray
} tmode_color_t;

typedef struct tmode_char_t {
    uint8_t acode;
    uint8_t atti;
} tmode_char_t;

void init_disp(void);
void clear_screen(void);
void set_cursor(uint16_t);
uint16_t get_cursor(void);
void kprint_char(char);

#endif
