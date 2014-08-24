/******************************************************************************
 *
 * uart3.c - UART test with C library support
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
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>

/* We will run the unit test of serial driver in main() */
int main(int argc, char *argv[])
{
	int c = 0, i = 0;
	char *buffer = 0;

	/* Unit test 1: write() */
	write(STDERR_FILENO, "Hello, World!\n1", 15);

	while (1) {
		/* Unit test 2: malloc() */
		buffer = malloc(128 + i*16);

		/* Unit test 3: printf() */
		printf(". buffer=%x\n", (unsigned int)buffer);

		/* Unit test 4: getchar(), this is a blocking read */
		c = getchar();

		/* Unit test 5: putchar() */
		putchar(c);

		/* Unit test 6: free() */
		if(c != 's') {
			free(buffer);
		}

		if(c == 'q') {
			printf("\nExit from main()...\n");
			break;
		}
		i++;
	}

	return 1;
}
