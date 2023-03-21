#include "mm.h"

static ppg_range_t kphymm;
// `MM_BASE` used for base of management struct
static ppg_t phymm_available;

void
init_phymm_range(void) {
    kphymm.ppg_base = kphymm.ppg_end
        = (uint8_t *)MM_BASE;
    kphymm.pg_amount = 0;
}

void
init_phymm(void) {
    // initialize the management
    init_phymm_range();
#ifdef DEBUG
    kprintf("kphymm:\n\tppg_base = %p; ppg_end = %p;"
        " pg_amount = %p\n\n\n", kphymm.ppg_base,
        kphymm.ppg_end, kphymm.pg_amount);
#endif

    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (size_t i = 0; i < *ards_num; ++i) {
        if (((ards + i)->type == ards_type_os)
        && ((ards + i)->base_low >= MM_BASE)) {
#ifdef DEBUG
    kprintf("ARDS:\n\tbase_low = %p; length_low = %p\n\n\n",
        (ards + i)->base_low, (ards + i)->length_low);
#endif
            uint32_t pages = (ards + i)->length_low / PGSIZE;
            kphymm.pg_amount = pages;
            kphymm.ppg_end += pages * 8; // use 8-bit to represent a page
        }
    }

#ifdef DEBUG
    // print
    kprintf("kphymm after ARDS travesaling:\n\tppg_base = %p;"
        " ppg_end = %p; pg_amount = %p\n\n\n", kphymm.ppg_base,
        kphymm.ppg_end, kphymm.pg_amount);
#endif

    memset((uint8_t *)MM_BASE, 0, kphymm.ppg_end - kphymm.ppg_base);

    // NOTE:
    // from `MM_BASE` to `kphymm.ppg_end` is used for management struct(i.e. `ppg_t`)
    // frome `kphymm.ppg_end` to maybe 0xffff_ffff is the real page available
    // initialize the `ppg_t` object
    ppg_t *cur = (ppg_t *)MM_BASE;
    ppg_t *worker = &phymm_available;
    uint8_t *curpg = (uint8_t *)PGUP((uint32_t)kphymm.ppg_end, PGSIZE);
    size_t skip = ((uint32_t)curpg - (uint32_t)cur) / PGSIZE;   // we must skip some pages used for management struct
#ifdef DEBUG
    kprintf("cur = %p; worker = %p; curpg = %p; skip = %d\n\n\n", cur,
        worker, curpg, skip);
#endif

    for (size_t i = skip; i < kphymm.pg_amount; ++i, curpg += PGSIZE) {
        cur[i].pgaddr = curpg;
        worker->next = cur + i;
        worker = cur + i;
    }

#ifdef DEBUG
    worker = &phymm_available;
    for (size_t i = 0; worker; ++i, worker = worker->next) {
        kprintf("[%d]addr: %p; next: %p", i, worker->pgaddr, worker->next);
        if (i % 2 == 0)    kprintf("\n");
        else    kprintf("\t\t");
    }
#endif
}
