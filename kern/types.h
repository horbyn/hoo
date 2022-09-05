#ifndef __KERN_TYPES_H__
#define __KERN_TYPES_H__

#ifndef null
#define null 0
#endif

#ifndef true
#define true  0
#define false 0
#endif

typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef unsigned short uint16_t;
typedef          short int16_t;
typedef unsigned char  uint8_t;
typedef          char  int8_t;

typedef uint8_t  byte;
typedef uint16_t word;
typedef uint32_t dword;

#endif