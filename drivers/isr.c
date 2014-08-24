/*****************************************************************************
* Product: GNU toolset for ARM, BSP for AT91SAM7S-EK
* Date of the Last Update:  Jun 29, 2007
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2007 Quantum Leaps, LLC. All rights reserved.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "hardware.h"
#include "timer.h"
#include "bsp.h"
#include "isr.h"
#include "serial_goldfish.h"

/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
void BSP_irq(void) {
    int irq = 0;

	irq = goldfish_get_irq_num();

	goldfish_mask_irq(irq);					/* Block the same IRQ before IRQ handler return. */

    asm("MSR cpsr_c,#(0x1F)");              /* allow nesting interrupts */
    irq_handler(irq);                       /* call the IRQ handler via the pointer to function */
    asm("MSR cpsr_c,#(0x1F | 0x80)");       /* lock IRQ before return */

    goldfish_unmask_irq(irq);				/* Enable IRQ, when complete IRQ handling. */
}

/*..........................................................................*/
__attribute__ ((section (".text.fastcode")))
void BSP_fiq(void) {                                              /* FIQ ISR*/
    /* Handle the FIQ directly. No AIC vectoring overhead necessary */

    /* (void)dummy;         suppress warning "dummy" was set but never used */
}

