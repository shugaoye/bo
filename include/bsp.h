/******************************************************************************
 *
 * bsp.h - Board support package for Android emulator (goldfish platform)
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
#ifndef bsp_h
#define bsp_h
#include <stdint.h>

typedef __signed__ char int8_t;
typedef unsigned char uint8_t;

typedef __signed__ short int16_t;
typedef unsigned short uint16_t;
/*
typedef __signed__ int int32_t;
typedef unsigned int uint32_t;

typedef __signed__ long int64_t;
typedef unsigned long u_int64_t;

typedef unsigned long uint64_t;
*/
typedef unsigned long ulong;

#define __iomem
#define __force

/*
 * Interrupt controller functions
 * */
void goldfish_mask_irq(unsigned int irq);
void goldfish_unmask_irq(unsigned int irq);
void goldfish_disable_all_irq(void);
int goldfish_get_irq_num(void);
int goldfish_irq_status(void);

/*
 * Timer functions
 * */
void goldfish_set_timer(unsigned long cycles);
void goldfish_clear_timer_int(void);
unsigned long goldfish_timer_read(void);
void goldfish_clear_alarm(void);

int timer_init (void);
ulong get_timer (ulong base);
void reset_timer_masked (void);
ulong get_timer_masked (void);
unsigned long long get_ticks(void);
unsigned long get_second(void);
unsigned long get_millisecond(void);
void __udelay (unsigned long usec);
ulong get_tbclk (void);

void BSP_init(void);         /* initialization of the board support package */
void irq_handler(int irq);   /* IRQ handler in system mode */

static inline unsigned char readb(const volatile void __iomem *addr)
{
	return (*(volatile unsigned char __force *) (addr));
}
static inline unsigned short readw(const volatile void __iomem *addr)
{
	return *(volatile unsigned short __force *) addr;
}
static inline unsigned int readl(const volatile void __iomem *addr)
{
	return *(volatile unsigned int __force *) addr;
}
static inline unsigned long long readq(const volatile void __iomem *addr)
{
	return *(volatile unsigned long long __force *) addr;
}

static inline void writeb(unsigned char b, volatile void __iomem *addr)
{
	*(volatile unsigned char __force *) addr = b;
}
static inline void writew(unsigned short b, volatile void __iomem *addr)
{
	*(volatile unsigned short __force *) addr = b;
}
static inline void writel(unsigned int b, volatile void __iomem *addr)
{
	*(volatile unsigned int __force *) addr = b;
}
static inline void writeq(unsigned int b, volatile void __iomem *addr)
{
	*(volatile unsigned long long __force *) addr = b;
}

#ifdef __USE_CLIB__
int printf ( const char * format, ... );
#define debug(fmt, args...) printf(fmt, ## args)
#else
#define debug(fmt, args...)
#endif /* __USE_CLIB__ */

void EnterUserMode(void);
void SystemCall(void);

#endif /* bsp_h */

