#include "mm.h"

static prange_t kphymm;
//static ppg_t *phymm_available, *phymm_used; // maybe the later is unnecessary?

void
init_phymm_range(void) {
    kphymm.base = kphymm.end
        = (uint8_t *)MM_BASE;
    kphymm.pg_amount = 0;
    kphymm.ppg_amount = 0;
}

void
init_phymm(void) {
    // initialize the management
    init_phymm_range();


    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;
    size_t num = *ards_num;
#ifdef DEBUG
    kprintf("ARDS amount is: %d\n", num);
#endif


    for (size_t i = 0; i < num; ++i) {
#ifdef DEBUG
    kprint_int(i);
#endif
        if (((ards + i)->type == ards_type_os)
        && ((ards + i)->base_low >= MM_BASE)) {
            /*TODO
            uint32_t length = (ards + i)->length_low;
            kphymm.end += length / PGSIZE;
            kphymm.pg_amount = length / PGSIZE;*/
        }
    }

    // print
    kprintf("available mm from %x to %x\n",
        (uint32_t)kphymm.base, (uint32_t)kphymm.end);
}
