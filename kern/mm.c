#include "mm.h"

void
get_phymm(void) {
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    size_t num = *ards_num;
    kprint_str("ards total: ");
    kprint_int(num);
    kprint_char('\n');

    for (size_t i = 0; i < num; ++i) {
        kprint_str("====================\nbase: 0x");
        kprint_hex(ards->base_low);
        kprint_str("\nlength: 0x");
        kprint_hex(ards->length_low);
        kprint_str("\ntype: ");
        kprint_hex(ards->type);
        kprint_str("\n\n");
    }
}
