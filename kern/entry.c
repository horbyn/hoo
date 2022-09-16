#include "disp.h"

int entry(void) {
    init_disp();
    kprint_str("welcome kernel!\n");

    // should not return
    return 0;
}