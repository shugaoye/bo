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
#ifndef __BARE_METAL__
#include <common.h>
#include <serial.h>
#include <configs/goldfish.h>
#else
#include <bsp.h>
#define NULL 0
#include <hardware.h>
#endif

#include "serial_goldfish.h"

/*
 * There are a few UART in goldfish. UART 0, UART 1 and UART 2.
 * When emulator is running with argument -shell, UART 2 is used as standard input/output device.
 * UART base addresses:
 * 0 - 0xff002000
 * 1 - 0xff011000
 * 2 - 0xff012000
 * */

void *getbaseaddr(void);
static struct goldfish_tty gtty = {0, 0};

static int goldfish_init(void)
{
	gtty.base = getbaseaddr();
	debug ("goldfish_init(), gtty.base=%x\n", gtty.base);
	writel(GOLDFISH_TTY_CMD_INT_ENABLE, (void *)IO_ADDRESS(GOLDFISH_TTY2_BASE) + GOLDFISH_TTY_CMD);

	return 0;
}

static int goldfish_disable_tty(void)
{
	writel(GOLDFISH_TTY_CMD_INT_DISABLE, (void *)IO_ADDRESS(GOLDFISH_TTY2_BASE) + GOLDFISH_TTY_CMD);

	return 0;
}

static void goldfish_gets(char *s, int len)
{
	void *base = 0;

	base = gtty.base;
	if(!base) {
		goldfish_init();
		base = gtty.base;
	}

	*((uint32_t *)(base + GOLDFISH_TTY_DATA_PTR)) = (uint32_t)s;
	*((uint32_t *)(base + GOLDFISH_TTY_DATA_LEN)) = len;
	*((uint32_t *)(base + GOLDFISH_TTY_CMD)) = GOLDFISH_TTY_CMD_READ_BUFFER;
}

static void goldfish_putc(const char c)
{
	void *base = 0;

	base = gtty.base;
	if(!base) {
		goldfish_init();
		base = gtty.base;
	}

	if(c) {
		*((uint32_t *)(base + GOLDFISH_TTY_PUT_CHAR)) = (uint32_t)c;
	}
}

static int goldfish_getc(void)
{
	char buf[128];
	uint32_t count;
	unsigned int data = 0;
	void *base = 0;

	base = gtty.base;
	if(!base) {
		goldfish_init();
		base = gtty.base;
	}
	if(base) {
		count = *((int *)(base + GOLDFISH_TTY_BYTES_READY));
		if(count <= 0) {
			debug ("Error: goldfish_getc(), count=%d\n", count);
			return -1;
		}

		goldfish_gets(buf, 1);
		data = buf[0];
	}

	return (int) data;
}

static int goldfish_tstc(void)
{
	int count = 0;
	void *base = 0;

	base = gtty.base;
	if(!base) {
		goldfish_init();
		base = gtty.base;
	}

	if(base) {
		count = *((int *)(base + GOLDFISH_TTY_BYTES_READY));
		if(count < 0) {
			debug ("Error: goldfish_tstc(), gtty.base=%x, base=%x, count=%d\n", gtty.base, base, count);
		}
	}

	return count;
}

static void goldfish_setbrg(void)
{
	debug("goldfish_setbrg()\n");
}

static struct serial_device goldfish_drv = {
	.name	= "goldfish_serial",
	.start	= goldfish_init,
	.stop	= goldfish_disable_tty,
	.setbrg	= goldfish_setbrg,
	.putc	= goldfish_putc,
	.puts	= default_serial_puts,
	.getc	= goldfish_getc,
	.tstc	= goldfish_tstc,
};

struct serial_device *default_serial_console(void)
{
	return &goldfish_drv;
}

void goldfish_initialize(void)
{
	debug("goldfish_initialize()\n");

	// serial_register(&goldfish_drv);
}

#ifdef __BARE_METAL__
void default_serial_puts(const char *s)
{
	struct serial_device *dev = &goldfish_drv;
		while (*s)
			dev->putc(*s++);
}

#endif
