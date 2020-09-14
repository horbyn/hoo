#include "types.h"
#include "display.h"
#include "alg.h"

int 
kernel_enter(void)
{
	char ch[2000] = { 0 };
	uint8_t *p = (uint8_t *)0xb8000;

	clear_screen();
	generate_char(ch);

	for (int i = 0; i < 4000; i += 2)
	{
		*(p + i) = ch[i / 2];
		*(p + i + 1) = 0xf;
	}

	scroll_screen();

	/*print_char('H');
	print_char('e');
	print_char('l');
	print_char('l');
	print_char('o');
	print_char(' ');
	print_char('w');
	print_char('o');
	print_char('r');
	print_char('l');
	print_char('d');
	print_char('!');

	print_num(329, 10);*/

	//get_cursor();

	//set_cursor(80);
	//get_cursor();

	return 0;
}
