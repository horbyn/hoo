#include "types.h"
#include "port.h"

// global cursor is stored at 0x80100
static uint16_t *g_Cursor = (uint16_t *)0x80100;

void 
get_cursor(void)
{
	//WORD cursor;

	outb(0xe, 0x3d4);
	//cursor = inb(0x3d5) << 8;
	*g_Cursor = (uint16_t)0 | inb(0x3d5) << 8;
	outb(0xf, 0x3d4);
	//cursor = inb(0x3d5);
	*g_Cursor = (uint16_t)inb(0x3d5);
}

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
