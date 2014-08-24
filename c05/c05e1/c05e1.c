/******************************************************************************
 *
 * c05e1.c - example of using C language in bare metal environment
 *
 * Copyright (c) 2013 Roger Ye.  All rights reserved.
 * Software License Agreement
 *
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 * NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 * NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. The AUTHOR SHALL NOT, UNDER
 * ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *****************************************************************************/

#define NULL 0

const int UART_IO_BASE = 0xff002000;

char *uart_name = "goldfish UART 1";

/* remove this, when port to u-boot */
struct serial_device {
	/* enough bytes to match alignment of following func pointer */
	char	name[16];

	int	(*start)(void);
	int	(*stop)(void);
	void	(*setbrg)(void);
	int	(*getc)(void);
	int	(*tstc)(void);
	void	(*putc)(const char c);
	void	(*puts)(const char *s);
	struct serial_device	*next;
};

struct serial_device goldfish_drv;

int strlen(const char * s)
{
	const char *sc;

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

int main(int argc, char *argv[])
{
	int i = 0, len = 0;

	if(argc > 0) {
		for (i = 0; i < argc; i++) {
			len = strlen(argv[i]);
		}
	}

	len = strlen(uart_name);

	return len;
}
