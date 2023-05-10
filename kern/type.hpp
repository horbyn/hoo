#pragma once
#ifndef __KERN_TYPE_HPP__
#define __KERN_TYPE_HPP__

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))             // calculate the array size

namespace hoo {

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;

}

#endif
