#include "mm.h"

void
get_phymm(void) {
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    size_t num = *ards_num;
    kprint_str("ards total: ");
    kprint_int(num);
    kprint_str("\n\n");

    for (size_t i = 0; i < num; ++i) {
        kprint_str("====================\nbase: 0x");
        kprint_hex((ards + i)->base_low);
        kprint_str("\tlength: 0x");
        kprint_hex((ards + i)->length_low);
        kprint_str("\ttype: ");
        kprint_hex((ards + i)->type);
        kprint_str("\n\n");
    }
}
