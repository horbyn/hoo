#include "mm.h"

static link_phy_t klink_phymm;

void
show_phymm(void) {
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

void
init_phymm(void) {
    klink_phymm.next = null;
    uint32_t *ards_num = (uint32_t *)ADDR_ARDS_NUM;
    ards_t *ards = (ards_t *)ADDR_ARDS_BASE;

    size_t num = *ards_num;
    for (size_t i = 0; i < num; ++i) {
        if ((ards + i)->type == ards_type_os) {
            uint32_t base = (ards + i)->base_low;
            uint32_t end = base + (ards + i)->length_low;

            base = PGUP(base, PGSIZE);
            end = PGDOWN(end, PGSIZE);
            for (; base < end; base += PGSIZE) {
                //kprint_hex(base);
                //kprint_char('\n');
                static link_phy_t temp;
                temp.addr = base;
                temp.next = klink_phymm.next;
                klink_phymm.next = &temp;
            }
        }
    }

    // print
    link_phy_t *ptr = klink_phymm.next;
    int i = 1;
    while (ptr != null) {
        kprint_int(i++);
        kprint_str(": ");
        kprint_hex(ptr->addr);
        kprint_char('\n');
        ptr = ptr->next;
    }
}
