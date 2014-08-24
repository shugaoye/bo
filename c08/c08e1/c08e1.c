/******************************************************************************
 *
 * c08e1.c - Example code for memory check
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
#include <string.h>
#include <sys/unistd.h>
#include <hardware.h>
#include <arm_exc.h>
#include <bsp.h>
#include "div64.h"
#include "mtd.h"

void EnterUserMode(void);
void SystemCall(void);

struct goldfish_nand {
	unsigned char __iomem  *base;
	size_t                  mtd_count;
	struct mtd_info         *mtd;
};

extern struct goldfish_nand goldfish_nand_info;

uint32_t __div64_32(uint64_t *n, uint32_t base)
{
	uint64_t rem = *n;
	uint64_t b = base;
	uint64_t res, d = 1;
	uint32_t high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (uint64_t) high << 32;
		rem -= (uint64_t) (high*base) << 32;
	}

	while ((int64_t)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}

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
		printf("=>IRQ_TTY2.\n");
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
	tm = get_ticks();
	printf("=>Exit ARM_irq(%d). tm=%lu\n", irq, tm);
}

/* We will run the unit test of serial driver in main() */
int main(int argc, char *argv[])
{
	struct mtd_info *mtd;
	int i;
	int num_dev = 0;

	printf("Enter main(), test goldfish NAND flash ...\n");

	EnterUserMode();

	/* Initialize NAND devices. */
	board_nand_init();

	num_dev = goldfish_nand_info.mtd_count;
	if(num_dev > 0) {
		for(i = 0; i < num_dev; i++) {
			mtd = &goldfish_nand_info.mtd[i];
			printf("Device %d %s\n", i, mtd->name);
			printf("Flags............. 0x%x %s\n", i, mtd->flags);
			printf("Size.............. %d\n", mtd->size);
			printf("Block/Page........ %d\n", mtd->erasesize/mtd->writesize);
			printf("Block Size........ %d\n", mtd->erasesize);
			printf("No. of Blocks..... %d\n", mtd->size/mtd->erasesize);
			printf("Page Size......... %d\n", mtd->writesize);
			printf("No. of Pages...... %d\n", mtd->size/mtd->writesize);
			printf("Extra Data Size... %d\n\n", mtd->oobsize);
		}
	}

	while(1) {};

	return 1;
}
