/******************************************************************************
 *
 * c07e1.c - Example code for interrupt handling
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
#include <hardware.h>
#include <arm_exc.h>
#include <bsp.h>

static int ch = 0;

void sw_handler(int num)
{
	printf("=>Inside sw_handler, num=%d.\n", num);
}

void irq_handler(int irq)
{
	int num = 0;
	unsigned long tm = 0;

	num = goldfish_irq_status();
	printf("=>Enter ARM_irq(%d), pending num=%d\n", irq, num);

	switch (irq) {
	case IRQ_TTY0:
		printf("=>IRQ_TTY0.\n");
		break;
	case IRQ_TTY1:
		printf("=>IRQ_TTY1.\n");
		break;
	case IRQ_TTY2:
	    ch = getchar();
		printf("=>IRQ_TTY2. ch=%c\n", ch);
		if(ch == 't') {
			/* Unit test 1: set timer to trigger timer interrupt. */
			goldfish_set_timer(0);
		}
		break;
	case IRQ_TIMER:
		/* goldfish_mask_irq(IRQ_TIMER); */
		goldfish_clear_timer_int();
		printf("=>IRQ_TIMER - clear interrupt.\n");
		break;
	default:
		printf("=>Unknown IRQ %x.\n", irq);
		break;
	}
	tm = goldfish_timer_read();
	printf("=>Exit ARM_irq(%d). tm=%lu\n", irq, tm);
}

/* We will run the unit test of serial driver in main() */
int main(int argc, char *argv[])
{
	unsigned long tm;
	int i = 0;

	printf("Enter main() ...\n");

    ARM_INT_UNLOCK(0x1F);            /* unlock IRQ/FIQ at the ARM core level */
	goldfish_unmask_irq(IRQ_TIMER);
	goldfish_unmask_irq(IRQ_TTY2);

	EnterUserMode();

	/* We do nothing in main() */
	for(;;) {
		if(ch != 0) {
			printf("1. Command is %c. time=(%lu).\n", ch, tm);

			if(ch == 'd') {
				/* disable timer interrupt. */
				goldfish_mask_irq(IRQ_TIMER);
				printf("  - Disabled timer.\n");
			}

			if(ch == 'e') {
				/* disable timer interrupt. */
				goldfish_unmask_irq(IRQ_TIMER);
				printf("  - Enabled timer.\n");
			}

			if(ch == 's') {
				/* Unit test 2: Fire a system call. */
				SystemCall();
				printf("  - Make system call.\n");
			}

		    ch = 0;
		    tm = goldfish_timer_read();
		    printf("2. ----- End loop (%lu) -----\n", tm);
		}
		else {
			tm = goldfish_timer_read();
		}
	}
	return 1;
}
