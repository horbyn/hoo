#pragma once
#ifndef __KERN_MEM_METADATA_H__
#define __KERN_MEM_METADATA_H__

#include "vspace.h"

void     init_metadata();
vspace_t *vspace_alloc();
node_t   *node_alloc();
vaddr_t  *vaddr_alloc();
void     vspace_release(vspace_t *vs);
void     node_release(node_t *node);
void     vaddr_release(vaddr_t *vaddr);

#endif
