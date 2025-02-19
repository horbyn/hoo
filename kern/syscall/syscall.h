#pragma once
#ifndef __KERN_SYSCALL_SYSCALL_H__
#define __KERN_SYSCALL_SYSCALL_H__

#define MAX_SYSCALL 32
typedef void (*syscall_t)(void);
extern syscall_t __stub[MAX_SYSCALL];

void syscall_init(void);

#endif
