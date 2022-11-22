#ifndef __KERN_MM_H__
#define __KERN_MM_H__

#include "types.h"
#include "disp.h"

// MUST be equal of `boot/bootsect.s`
#define ADDR_ARDS_NUM   0xf004
#define ADDR_ARDS_BASE  0xf008
#define PGSIZE          4096
#define PGDOWN(x, align) \
    ((x) & ~(align - 1))
#define PGUP(x, align) \
    (PGDOWN((x + align - 1), align))

typedef struct {
    uint32_t base_low;
    uint32_t base_hig;
    uint32_t length_low;
    uint32_t length_hig;
    uint32_t type;
} __attribute__((packed)) ards_t;

typedef enum {
    ards_type_os = 1,   // OS can use
    ards_type_arch      // arch reserves
} ards_type_t;

typedef struct link_phy {
    struct link_phy *next;
} link_phy_t;

void show_phymm(void);
void init_phymm(void);

#endif
