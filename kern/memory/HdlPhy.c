/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "HdlPhy.h"

static ppg_range_t kphymm;
static ppg_t phymm_available;

static void
init_phymm_range(void) {
    kphymm.ppg_base = kphymm.ppg_end = (uint8_t *)MM_BASE;
    kphymm.pg_amount = 0;
}

void
init_phymm(void) {
    // initialize the management
    init_phymm_range();

    // travesal ARDS to find out the available mm.
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    for (size_t i = 0; i < *ards_num; ++i) {
        if (((ards + i)->type_ == ardstype_os)
        && ((ards + i)->base_low_ >= MM_BASE)) {
            uint32_t pages = (ards + i)->length_low_ / PGSIZE;
            kphymm.pg_amount = pages;
            // use 8-bit to represent a page
            kphymm.ppg_end += pages * 8;
        }
    }

    memset((uint8_t *)MM_BASE, 0, kphymm.ppg_end - kphymm.ppg_base);

    // NOTE:
    // from `MM_BASE` to `kphymm.ppg_end` is used for management struct(i.e. `ppg_t`)
    // from `kphymm.ppg_end` to maybe 0xffff_ffff is the real page available
    // initialize the `ppg_t` object
    ppg_t *cur = (ppg_t *)MM_BASE;
    ppg_t *worker = &phymm_available;
    uint8_t *curpg = (uint8_t *)PGUP((uint32_t)kphymm.ppg_end, PGSIZE);
    // we must skip some pages used for management struct
    size_t skip = ((uint32_t)curpg - (uint32_t)cur) / PGSIZE;

    // set the linked list for all the physical mm.
    for (size_t i = skip; i < kphymm.pg_amount; ++i, curpg += PGSIZE) {
        cur[i].pgaddr = curpg;
        worker->next = cur + i;
        worker = cur + i;
    }

}
