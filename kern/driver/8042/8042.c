/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "8042.h"

static char keymap[][2] = {
     /*0*/{ 0, 0 },        /*1*/{ '\x7e', '\x7e' }, /*2*/{ '1', '!' },
     /*3*/{ '2', '@' },    /*4*/{ '3', '#' },       /*5*/{ '4', '$' },
     /*6*/{ '5', '%' },    /*7*/{ '6', '^' },       /*8*/{ '7', '&' },
     /*9*/{ '8', '*' },   /*10*/{ '9', '(' },      /*11*/{ '0', ')' },
    /*12*/{ '-', '_' },   /*13*/{ '=', '+' },      /*14*/{ '\b', '\b' },
    /*15*/{ '\t', '\t' }, /*16*/{ 'q', 'Q' },      /*17*/{ 'w', 'W' },
    /*18*/{ 'e', 'E' },   /*19*/{ 'r', 'R' },      /*20*/{ 't', 'T' },
    /*21*/{ 'y', 'Y' },   /*22*/{ 'u', 'U' },      /*23*/{ 'i', 'I' },
    /*24*/{ 'o', 'O' },   /*25*/{ 'p', 'P' },      /*26*/{ '[', '{' },
    /*27*/{ ']', '}' },   /*28*/{ '\n', '\n' },    /*29*/{ 0, 0 }/*left ctrl*/,
    /*30*/{ 'a', 'A' },   /*31*/{ 's', 'S' },      /*32*/{ 'd', 'D' },
    /*33*/{ 'f', 'F' },   /*34*/{ 'g', 'G' },      /*35*/{ 'h', 'H' },
    /*36*/{ 'j', 'J' },   /*37*/{ 'k', 'K' },      /*38*/{ 'l', 'L' },
    /*39*/{ ';', ':' },   /*40*/{ '\'', '\"' },    /*41*/{ '`', '~' },
    /*42*/{ 0, 0 }/*left shift*/,                  /*43*/{ '\\', '|' },
    /*44*/{ 'z', 'Z' },   /*45*/{ 'x', 'X' },      /*46*/{ 'c', 'C' },
    /*47*/{ 'v', 'V' },   /*48*/{ 'b', 'B' },      /*49*/{ 'n', 'N' },
    /*50*/{ 'm', 'M' },   /*51*/{ ',', '<' },      /*52*/{ '.', '>' },
    /*53*/{ '/', '?' },   /*54*/{ 0, 0 }/*right shift*/,
    /*55*/{ '*', '*' },   /*56*/{ 0, 0 }/*left alt*/,
    /*57*/{ ' ', ' ' },   /*58*/{ 0, 0 }/*capslock*/,
    { 0, 0 }
};
static bool gflag_make_shift, gflag_make_caps;
static cclbuff_t *kb_buff;

/**
 * @brief get the keyboard data buffer
 * 
 * @return keyboard data buffer
 */
cclbuff_t *
get_kb_buff(void) {
    return kb_buff;
}

/**
 * @brief initialize the 8042 keyboard controller
 */
void
init_8042(void) {
    kb_buff = cclbuff_alloc(MAXSIZE_KBBUFF);
}

/**
 * @brief PS/2 interrupt handler
 */
void
ps2_intr(void) {
    uint8_t ch = inb(DATA_PORT_8042);
    if (ch < 1 || (59 < ch && ch < BREAK_LSHIFT)
        || (BREAK_LSHIFT < ch && ch < BREAK_RSHIFT) || (ch > BREAK_RSHIFT))
        return;

    // control key
    if (ch == MAKE_LSHIFT || ch == MAKE_RSHIFT) {
        gflag_make_shift = true;
        return;
    }
    if (ch == MAKE_CAPSLOCK) {
        gflag_make_caps = !gflag_make_caps;
        return;
    }
    if (ch == BREAK_LSHIFT || ch == BREAK_RSHIFT) {
        gflag_make_shift = false;
        return;
    }

    char result = 0;
    if ((16 <= ch && ch <= 25) || (30 <= ch && ch <= 38) || (44 <= ch && ch <= 50)) {
        if (gflag_make_caps)    result = keymap[ch][1];
        else if (gflag_make_shift)    result = keymap[ch][1];
        else    result = keymap[ch][0];
    } else {
        if (gflag_make_shift)    result = keymap[ch][1];
        else    result = keymap[ch][0];
    }

    cclbuff_t *kbuff = get_kb_buff();
    if (cclbuff_put(kbuff, result) == false)
        kprintf("too much characters input in one time");
}
