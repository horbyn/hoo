/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2024   *
 *           (horbyn@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __BOOT_KERN_WILL_USE_H__
#define __BOOT_KERN_WILL_USE_H__

/*
 * ******************************************************************
 * ************** the following layout used for boot ****************
 *                                                                  *
 * 0x77ffc          0x78000   0x78200  0x78204  0x78400     0x80000 *
 * ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ *
 *       │DIED INSTRUCTION│   MBR   │ARDS NUM│  ARDS  │   STACK   │ *
 * ──────┼────────────────┼─────────┼────────┼────────┼───────────┤ *
 *                                                                  *
 * ******************************************************************
 */

// kernel stack
//     Althrough it looks like the stack is set to 0x70000,
//     the sp(esp) is subtracted first you know.
//     So if sp(esp) was 0 and then push stack, you will get
//     ss:sp(esp) that 0x7000:0xfffe(0xfffc)
//
//     This is why 0x7000 below
#define SEG_KSTACK  0x7000

// here sets the `jmp .` instruction
#define SEG_DIED    0x77f0
#define OFF_DIED    0xfc

// here record the ARDS struct temporary
#define SEG_ARDS    0x7820
#define OFF_ARDS_CR 0
#define OFF_ARDS    4

#endif
