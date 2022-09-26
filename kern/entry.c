#include "disp.h"

int entry(void) {
    init_disp();
    kprint_str("a\nb\t\b");

    while (1);
    // should not return
    return 0;
}