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
#include <bsp.h>

static int ch = 0;

void __attribute__((interrupt)) ARM_irq(void) {
	int irq = 0, num = 0;
	unsigned long tm = 0;

	irq = goldfish_get_irq_num();
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
			/* Unit test 4: set timer to trigger timer interrupt. */
			goldfish_set_timer(0);
		}
		break;
	case IRQ_TIMER:
		/* We can clear either interrupt or alarm here.
		goldfish_clear_timer_int();
		printf("IRQ_TIMER - clear interrupt.\n");
		*/
		goldfish_clear_alarm();
		printf("=>IRQ_TIMER - clear alarm.\n");
		break;
	default:
		printf("=>Unknown IRQ %x.\n", irq);
		break;
	}
	tm = goldfish_timer_read();
	printf("=>Exit ARM_irq(%d). tm=%lu\n", irq, tm);
}

/* all other handlers are infinite loops */
void __attribute__((interrupt)) ARM_undef(void) {
	printf("Enter ARM_undef() ...\n");
	for(;;);
}

void __attribute__((interrupt)) ARM_swi(void) {
	printf("Enter ARM_swi() ...\n");
}

void __attribute__((interrupt)) ARM_pAbort(void) {
	printf("Enter ARM_pAbort() ...\n");
	for(;;);
}

void __attribute__((interrupt)) ARM_dAbort(void) {
	printf("Enter ARM_dAbort() ...\n");
	for(;;);
}

void __attribute__((interrupt)) ARM_reserved(void) {
	printf("Enter ARM_reserved() ...\n");
	for(;;);
}

void __attribute__((interrupt)) ARM_fiq(void) {
	printf("Enter ARM_fiq() ...\n");
	for(;;);
}

/* We will run the unit test of serial driver in main() */
int main(int argc, char *argv[])
{
	unsigned long int tm = 0;

	printf("Enter main() ...\n");

	goldfish_unmask_irq(IRQ_TIMER);
	goldfish_unmask_irq(IRQ_TTY2);

	/* We do nothing in main() */
	for(;;) {
		if(ch != 0) {
			/* Unit test 1: print out serial input and time stamp. */
			printf("1. Command is %c. time=(%lu).\n", ch, tm);

			if(ch == 'd') {
				/* Unit test 2: disable timer interrupt. */
				goldfish_mask_irq(IRQ_TIMER);
				printf("  - Disabled timer.\n");
			}

			if(ch == 'e') {
				/* Unit test 3: enable timer interrupt. */
				goldfish_unmask_irq(IRQ_TIMER);
				printf("  - Enabled timer.\n");
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
