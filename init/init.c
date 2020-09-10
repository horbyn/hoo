#include "types.h"
#include "display.h"

int 
kernel_enter(void)
{
	clear_screen();

	print_char('H');
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

	print_num(329, 10);

	//get_cursor();

	//set_cursor(80);
	//get_cursor();

	return 0;
}
