#ifndef __KERN_MM_H__
#define __KERN_MM_H__

#include "types.h"
#include "disp.h"

// MUST be equal of `boot/bootsect.s`
#define ADDR_ARDS_NUM   0xf004
#define ADDR_ARDS_BASE  0xf008
#define MM_BASE         0x100000
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

typedef struct phy_page {
    uint8_t *pgaddr;    // indicate the page represented
    struct phy_page *next;// indicate the next `ppg_t` node
} ppg_t;

typedef struct phymm_range {
    uint8_t *base, *end;// indicate where memory begin and end
    size_t pg_amount;   // indicate pages amount
    size_t ppg_amount;  // indicate the amount of pages management
} prange_t;

void init_phymm_range(void);
void init_phymm(void);

#endif
