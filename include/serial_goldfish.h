/******************************************************************************
 *
 * serial_goldfish.h - header file for goldfish serial port
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

#ifdef __BARE_METAL__
/* remove this, when port to u-boot */
#define uint32_t unsigned int

/* 
#ifdef __USE_CLIB__
int printf ( const char * format, ... );
#define debug(fmt, args...) printf(fmt, ## args)
#else
#define debug(fmt, args...)
#endif __USE_CLIB__ */

struct serial_device *default_serial_console(void);
void default_serial_puts(const char *s);

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
#endif /* __BARE_METAL__ */

struct goldfish_tty {
	void *base;
	int opencount;
};

/*
 * The serial controller has a 4KiB block of registers residing at 0xfe000000.
 * It consist of 5 32-bit registers.
 * PUT_CHAR at offset 0x0 is a write-only register. Writing a value to it puts a character onto the console.
 * BYTES_READY at offset 0x4 returns the number of characters waiting to be read from the console. This register is read-only.
 * CMD at offset 0x8 is a write-only register. Writing a command performs one of four actions.
 *   CMD_INT_DISABLE (0) disables the console interrupt.
 *   CMD_INT_ENABLE (1) enables the console interrupt.
 *   CMD_WRITE_BUFFER (2) copies DATA_LEN bytes from virtual address DATA_PTR to the console.
 *   CMD_READ_BUFFER (3) copies DATA_LEN bytes from the console to virtual address DATA_PTR.
 *   The number of bytes should not exceed that specified byBYTES_READY.
 */

enum {
	GOLDFISH_TTY_PUT_CHAR       = 0x00,
	GOLDFISH_TTY_BYTES_READY    = 0x04,
	GOLDFISH_TTY_CMD            = 0x08,

	GOLDFISH_TTY_DATA_PTR       = 0x10,
	GOLDFISH_TTY_DATA_LEN       = 0x14,

	GOLDFISH_TTY_CMD_INT_DISABLE    = 0,
	GOLDFISH_TTY_CMD_INT_ENABLE     = 1,
	GOLDFISH_TTY_CMD_WRITE_BUFFER   = 2,
	GOLDFISH_TTY_CMD_READ_BUFFER    = 3,
};

