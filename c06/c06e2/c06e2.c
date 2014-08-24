/******************************************************************************
 *
 * c06e2.c - Test code for semihosting C library support
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

int main(int argc, char *argv[])
{
	FILE *fp;
	int c = '1', i = 0;
	char *buffer = 0;

	/* Unit test 1: write() */
	write(STDERR_FILENO, "Hello, World!\n1", 15);

	/* Unit test 2: fopen() */
	fp = fopen("log.txt", "w");
	if(fp == NULL) return 0;

	while (1) {
		/* Unit test 3: malloc() */
		buffer = malloc(128 + i*16);

		/* Unit test 4: printf() */
		printf(". buffer=%x\n", (unsigned int)buffer);
		fprintf(fp, "%c. buffer=%x\n", c, (unsigned int)buffer);

		/* Unit test 5: getchar(), this is a blocking read */
		do {
			c = getchar();
		} while (c == '\n');

		/* Unit test 6: putchar() */
		putchar(c);

		/* Unit test 7: free() */
		if(c != 's') {
			free(buffer);
		}

		if(c == 'q') {
			printf("\nExit from main()...\n");
			break;
		}
		i++;
	}

	/* Unit test 8: fclose() */
	fclose(fp);

	return 1;
}

