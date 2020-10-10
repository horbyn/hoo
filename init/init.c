#include "types.h"
#include "display.h"

int 
kernel_enter(void)
{
	clear_screen();

	for (int i = 0; i < 24; ++i)    print_str("Hello world!\n");
	print_str("Hello world!24");
	print_str("He");

	return 0;
}
