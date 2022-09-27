#include "disp.h"
#include "inte.h"

int entry(void) {
    init_disp();
    init_pic();

    while (1);
    // should not return
    return 0;
}