/******************************************************************************
 *
 * bsp.c - Board support package for Android emulator (goldfish platform)
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
#include <hardware.h>
#include <bsp.h>                                   /* Board Support Package */
#include <isr.h>                      /* interface to the ISRs (foreground) */

/* Refer to arch/arm/mach-goldfish/board-goldfish.c */

/*
 * GOLDFISH_INTERRUPT_DISABLE - at offset 0xC is a write-only register.
 * Writing an interrupt number to it will disable to specified interrupt.
 * */
void goldfish_mask_irq(unsigned int irq)
{
	writel(irq, (void *)IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_DISABLE);
}

/*
 * GOLDFISH_INTERRUPT_ENABLE - at offset 0x10 is a write-only register.
 * Writing an interrupt number to it will enable to specified interrupt.
 * */
void goldfish_unmask_irq(unsigned int irq)
{
	writel(irq, (void *)IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_ENABLE);
}

/*
 * GOLDFISH_INTERRUPT_DISABLE_ALL - at offset 0x8 is a write-only register.
 * Writing any value other than 0 to it will disable all interrupts.
 * */
void goldfish_disable_all_irq(void)
{
	writel(1, (void *)IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_DISABLE_ALL);
}

/*
 * GOLDFISH_INTERRUPT_NUMBER - at offset 0x4 contains the lowest pending,
 * enabled interrupt number. It is a read-only register.
 * */
int goldfish_get_irq_num(void)
{
	return readl((void *)IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_NUMBER);
}

/*uint32_t
 * GOLDFISH_INTERRUPT_STATUS - at offset 0x0 contains the number of pending interrupt.
 * It is a read-only register.
 * */
int goldfish_irq_status(void)
{
	return readl((void *)IO_ADDRESS(GOLDFISH_INTERRUPT_BASE) + GOLDFISH_INTERRUPT_STATUS);
}

/*..........................................................................*/
void BSP_init(void) {
    uint32_t int_base = IO_START + GOLDFISH_INTERRUPT_BASE;

    /* hook the exception handlers */
    *(uint32_t volatile *)0x24 = (uint32_t)&ARM_undef;
    *(uint32_t volatile *)0x28 = (uint32_t)&ARM_swi;
    *(uint32_t volatile *)0x2C = (uint32_t)&ARM_pAbort;
    *(uint32_t volatile *)0x30 = (uint32_t)&ARM_dAbort;
    *(uint32_t volatile *)0x34 = (uint32_t)&ARM_reserved;
    *(uint32_t volatile *)0x38 = (uint32_t)&ARM_irq;
    *(uint32_t volatile *)0x3C = (uint32_t)&ARM_fiq;

    /* configure goldfish Interrupt Controller */
    writel(1, (void *)int_base + GOLDFISH_INTERRUPT_DISABLE_ALL);

    /* ARM_INT_UNLOCK(0x1F);            unlock IRQ/FIQ at the ARM core level */
}
/*..........................................................................*/
void BSP_abort(char const *msg) {
    /* this function is called when an exception occurs.
    * For production code you need to log the message and go to fail-safe
    * state. You might also want to reset the CPU.
    */
	printf("=>BSP_about.\n");
    for (;;) {
    }
}
