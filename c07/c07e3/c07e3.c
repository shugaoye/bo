/******************************************************************************
 *
 * c07e3.c - Example code for goldfish RTC and timer
 *
 * Copyright (c) 2014 Roger Ye.  All rights reserved.
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

#include "rtc.h"

static int ch = 0;
static int timeout = 0;
static int timer_irq = 0;

void sw_handler(int num)
{
	printf("=>Inside sw_handler, num=%d.\n", num);
}

void irq_handler(int irq)
{
	int num = 0;
	unsigned long tm = 0;

	num = goldfish_irq_status();
	printf("\n<=Enter IRQ(%d), %d pending, ", irq, num);

	switch (irq) {
	case IRQ_TTY0:
		printf("=>IRQ_TTY0.\n");
		break;
	case IRQ_TTY1:
		printf("=>IRQ_TTY1.\n");
		break;
	case IRQ_TTY2:
	    ch = getchar();
		printf("IRQ_TTY2, ch=%c, ", ch);
		if(ch == 't') {
			/* Timer Unit test 1: set timeout to trigger timer interrupt. */
			printf("timeout = %d ", timeout);
			goldfish_set_timer(timeout);
			timeout = timeout + 1;
			timer_irq = 1;
		}

		/* Timer Unit test 2: change the timeout value of timer interrupt. */
		if(ch == 'x') {
			if(timeout > 0) {
				timeout = timeout * 10;
			}
		}

		/* RTC Unit test 3: reset date and time. */
		if(ch == 'r') {
			timeout = 0;
			rtc_reset();
		}
		break;
	case IRQ_TIMER:
		/* goldfish_mask_irq(IRQ_TIMER); */
		goldfish_clear_timer_int();
		printf("IRQ_TIMER ");
		timer_irq = 0;
		break;
	default:
		printf("=>Unknown IRQ %x.\n", irq);
		break;
	}
	tm = get_ticks();
	printf("Exit IRQ(%d) tm=%lu =>\n", irq, tm);
}

int main(int argc, char *argv[])
{
	struct rtc_time rtc;

	printf("Starting c07e3 ...\n");

    ARM_INT_UNLOCK(0x1F);            /* unlock IRQ/FIQ at the ARM core level */
	goldfish_unmask_irq(IRQ_TIMER);
	goldfish_unmask_irq(IRQ_TTY2);

	EnterUserMode();

	/* Initialize timer. */
	timer_init();

	for(;;) {
		if(ch != 0) {

			if(ch == 'd') {
				/* disable timer interrupt. */
				goldfish_mask_irq(IRQ_TIMER);
				printf("  - Disabled timer.\n");
			}

			if(ch == 'e') {
				/* Enable timer interrupt. */
				goldfish_unmask_irq(IRQ_TIMER);
				printf("  - Enabled timer.\n");
			}

			if(ch == 'g') {
				/* RTC Unit test 1: get date and time. */
				rtc_get(&rtc);
				printf("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d (%lu)\n",
					rtc.tm_year, rtc.tm_mon, rtc.tm_mday, rtc.tm_wday,
					rtc.tm_hour, rtc.tm_min, rtc.tm_sec, get_millisecond());
			}

			if(ch == 's') {
				/* RTC Unit test 2: Set the date to 2014-04-11 12:30:55. */
				rtc.tm_year = 2014;
				rtc.tm_mon = 4;
				rtc.tm_mday = 11;
				rtc.tm_hour = 12;
				rtc.tm_min = 30;
				rtc.tm_sec = 55;

				rtc_set(&rtc);
				printf("  - set RTC. ");
				printf("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d (%lu)\n",
					rtc.tm_year, rtc.tm_mon, rtc.tm_mday, rtc.tm_wday,
					rtc.tm_hour, rtc.tm_min, rtc.tm_sec, get_millisecond());
			}

		    ch = 0;
		}
		else {
			if(timeout > 1000 && timer_irq > 0) {
				/* Timer Unit test 3: print out debug message every one second for large timeout value. */
				rtc_get(&rtc);
				printf("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d (%lu)\n",
					rtc.tm_year, rtc.tm_mon, rtc.tm_mday, rtc.tm_wday,
					rtc.tm_hour, rtc.tm_min, rtc.tm_sec, get_millisecond());
				__udelay(1000000);
			}
			else {
				__udelay(1000);
			}
		}
	}
	return 1;
}
