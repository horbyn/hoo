/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "idle.h"

pgelem_t __pgdir_idle[PGDIR_SIZE] __attribute__((aligned(4096)));
pcb_t *__pcb_idle;
