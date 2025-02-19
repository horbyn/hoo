#pragma once
#ifndef __KERN_PANIC_H__
#define __KERN_PANIC_H__

extern void isr_part3(void);

void panic(const char *extra);

#endif
