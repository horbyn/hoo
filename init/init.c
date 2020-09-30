#include "types.h"
#include "display.h"

int 
kernel_enter(void)
{
	char j = '0';

	clear_screen();

	for (int i = 0; i < 15 * 80 + 35; ++i)
	{
		if (j == 58)    j = '0';
		print_char(j++);
	}

	print_char('\b');

	return 0;
}
