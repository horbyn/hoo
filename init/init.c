int kernel_enter(void)
{
	unsigned char *p = 0xb8000;

	*p++ = 'H';
	*p++ = 0x0f;
	*p++ = 'e';
	*p++ = 0x0f;
	*p++ = 'l';
	*p++ = 0x0f;
	*p++ = 'l';
	*p++ = 0x0f;
	*p++ = 'o';
	*p++ = 0x0f;
	*p++ = ' ';
	*p++ = 0x0f;
	*p++ = 'w';
	*p++ = 0x0f;
	*p++ = 'o';
	*p++ = 0x0f;
	*p++ = 'r';
	*p++ = 0x0f;
	*p++ = 'l';
	*p++ = 0x0f;
	*p++ = 'd';
	*p++ = 0x0f;
	*p++ = '!';
	*p++ = 0x0f;

	return 0;
}
