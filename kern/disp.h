#ifndef __KERN_DISP_H__
#define __KERN_DISP_H__

#include "types.h"
#include "x86.h"
#include "lib.h"

#define GLOBAL_CURSOR 0xf000    // global cursor
#define VIDEO_MEM     0xb8000   // video mm. base
#define VGA_WIDTH     80
#define VGA_HIGH      25

typedef char *va_list;
#define POINTER_SIZE (sizeof(const char *))
#define TYPE_TO_POINTER_SIZE(type) \
    ((sizeof(type) - 1) / POINTER_SIZE + 1)
// set `a` to the address of `fst`
#define va_start(a,fst) \
    ((a) = (((va_list)&(fst))))
// move `a` to next pointer
#define va_arg(a,type)  \
    ((a) = (a) + (sizeof(va_list) * TYPE_TO_POINTER_SIZE(type)))
// set `a` to NULL (seems not necessary?)
#define va_end(a)       \
    ((a) = (va_list)0)

typedef struct tmode_char_t {
    uint8_t acode;
    uint8_t atti;
} tmode_char_t;

void init_disp(void);
void clear_screen(void);
void set_cursor(int, int);
uint16_t get_cursor(void);
void kprint_char(char);
void kprint_str(const char *);
void kprint_int(int);
void kprint_hex(uint32_t);
void kprintf(const char *, ...);
void scroll_back(void);

#endif
