/* include/asm-arm/arch-goldfish/hardware.h
**
** Copyright (C) 2007 Google, Inc.
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
*/

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

/* include/asm-arm/arch-goldfish/irqs.h */
/* #define _ARCH-GOLDFISH_IRQS_H */
#define IRQ_PDEV_BUS    (1)
#define IRQ_TIMER       (3)
#define IRQ_TTY0		(4)
#define IRQ_RTC			(10)
#define IRQ_TTY1		(11)
#define IRQ_TTY2		(12)
#define IRQ_smc91x		(13)
#define IRQ_FB			(14)
#define IRQ_AUDIO		(15)
#define IRQ_EVENTS		(16)
#define IRQ_PIPE		(17)
#define IRQ_SWITCH0		(18)
#define IRQ_SWITCH1		(19)
#define IRQ_RANDOM 		(20)

#define LAST_IRQ RANDOM_IRQ
#define NR_IRQS (LAST_IRQ + 1)

/*
 * Where in virtual memory the IO devices (timers, system controllers
 * and so on)
 */
#define IO_BASE			0xfe000000                 // VA of IO 
#define IO_SIZE			0x00800000                 // How much?
#define IO_START		0xff000000                 // PA of IO

#define GOLDFISH_INTERRUPT_BASE         (0x0)
#define GOLDFISH_INTERRUPT_STATUS       (0x00) // number of pending interrupts
#define GOLDFISH_INTERRUPT_NUMBER       (0x04)
#define GOLDFISH_INTERRUPT_DISABLE_ALL  (0x08)
#define GOLDFISH_INTERRUPT_DISABLE      (0x0c)
#define GOLDFISH_INTERRUPT_ENABLE       (0x10)

#define GOLDFISH_PDEV_BUS_BASE      (0x1000)
#define GOLDFISH_PDEV_BUS_END       (0x100)

#define GOLDFISH_TTY_BASE       (0x2000)
#define GOLDFISH_TIMER_BASE     (0x3000)
#define GOLDFISH_AUDIO_BASE     (0x4000)
#define GOLDFISH_MMC_BASE		(0x5000)
#define GOLDFISH_MEMLOG_BASE    (0x6000)
#define GOLDFISH_RTC_BASE       (0x10000)
#define GOLDFISH_TTY1_BASE      (0x11000)
#define GOLDFISH_TTY2_BASE      (0x12000)
#define GOLDFISH_smc91x_BASE    (0x13000)
#define GOLDFISH_FB_BASE        (0x14000)
#define GOLDFISH_EVENTS_BASE    (0x16000)
#define GOLDFISH_NAND_BASE      (0x17000)
#define GOLDFISH_PIPE_BASE      (0x18000)
#define GOLDFISH_SWITCH0_BASE   (0X19000)
#define GOLDFISH_SWITCH1_BASE   (0x1a000)


/* macro to get at IO space when running virtually */
#define IO_ADDRESS(x) ((x) + IO_START)

/* U-Boot definitions */
#define CONFIG_SYS_HZ			1000

#endif /* __ASM_ARCH_HARDWARE_H */
