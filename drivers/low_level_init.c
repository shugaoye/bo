/*****************************************************************************
* Product: Low-level initialization for AT91SAM7S-EK
* Date of the Last Update:  May 16, 2007
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
#include "bsp.h"
void __attribute__((interrupt)) ARM_undef(void);
void __attribute__((interrupt)) ARM_swi(void);
void __attribute__((interrupt)) ARM_pAbort(void);
void __attribute__((interrupt)) ARM_dAbort(void);
void __attribute__((interrupt)) ARM_reserved(void);
void __attribute__((interrupt)) ARM_irq(void);
void __attribute__((interrupt)) ARM_fiq(void);
extern uint32_t __cs3_reset;

/*..........................................................................*/
/* low_level_init() is invoked by the startup sequence after initializing
* the C stack, but before initializing the segments in RAM.
*
* low_level_init() is invoked in the ARM state. The function gives the
* application a chance to perform early initializations of the hardware.
* This function cannot rely on initialization of any static variables,
* because these have not yet been initialized in RAM.
*/
void low_level_init(void (*reset_addr)(), void (*return_addr)()) {
    static uint32_t const LDR_PC_PC = 0xE59FF000U;
    static uint32_t const MAGIC = 0xDEADBEEFU;
    uint32_t m;
    *(uint32_t volatile *)(0x00) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x04) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x08) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x0C) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x10) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x14) = MAGIC;
    *(uint32_t volatile *)(0x18) = LDR_PC_PC | 0x18;
    *(uint32_t volatile *)(0x1C) = LDR_PC_PC | 0x18;

    /* setup the secondary vector table in RAM */
    *(uint32_t volatile *)(0x20) = (uint32_t)&__cs3_reset;
    *(uint32_t volatile *)0x24 = (uint32_t)&ARM_undef;
    *(uint32_t volatile *)0x28 = (uint32_t)&ARM_swi;
    *(uint32_t volatile *)0x2C = (uint32_t)&ARM_pAbort;
    *(uint32_t volatile *)0x30 = (uint32_t)&ARM_dAbort;
    *(uint32_t volatile *)0x34 = (uint32_t)&ARM_reserved;
    *(uint32_t volatile *)0x38 = (uint32_t)&ARM_irq;
    *(uint32_t volatile *)0x3C = (uint32_t)&ARM_fiq;

    /* check if the Memory Controller has been remapped already */
    m = *(uint32_t volatile *)0x14;
    if (MAGIC != m) {
    	/* AT91C_BASE_MC->MC_RCR = 1;    perform Memory Controller remapping */
    	m++;
    }
}

