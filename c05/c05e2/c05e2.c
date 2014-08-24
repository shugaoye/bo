/******************************************************************************
 *
 * serial_goldfish.c - U-Boot serial port implementation for goldfish
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

#include <serial_goldfish.h>

/* We will run the unit test of serial driver in main() */
int main(int argc, char *argv[])
{
	struct serial_device *drv;
	int c;

	/* Unit test 1: default_serial_console*/
	drv = default_serial_console();

	/* Unit test 2: goldfish_init */
	drv->start();

	/* Unit test 3: default_serial_puts & goldfish_putc */
	if(argc == 1) {
		drv->puts(argv[0]);
	}

	/* Unit test 4: goldfish_tstc */
	while (1) {
		if(drv->tstc()) {
		/* Unit test 5: goldfish_getc */
			c = drv->getc();
			drv->putc(c);
			if(c == 'q') break;
		}
	}

	return 1;
}
