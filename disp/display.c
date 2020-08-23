#include "types.h"

// global cursor is stored at 0x80100
static uint8_t *g_Cursor = (uint8_t *)0x80100;

void 
clear_screen(void)
{
	uint8_t *p = (uint8_t *)0xb8000;
	
	for (int i = 0; i < 25 * 80; ++i)
	{
		*p++ = ' ';
		*p++ = 0x0f;
	}

	*g_Cursor = 0;
}

static int 
print_char(char ch)
{
	return 0;
}
