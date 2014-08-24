/******************************************************************************
 *
 * startup_cs3 - Startup code to work with CS3 start sequence
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
    .text
    .code 32

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
    .equ    I_BIT,          0x80      /* when I bit is set, IRQ is disabled */
    .equ    F_BIT,          0x40      /* when F bit is set, FIQ is disabled */

    .equ    USR_MODE,       0x10
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SVC_MODE,       0x13
    .equ    ABT_MODE,       0x17
    .equ    UND_MODE,       0x1B
    .equ    SYS_MODE,       0x1F

    .global __cs3_reset 

__cs3_reset:  
	/* set Supervisor stack */
    LDR     r0,=__cs3_reset      /* pass the reset address as the 1st argument */
    LDR     r1,=__cs3_start_asm   /* pass the return address as the 2nd argument */
    MOV     lr,r1           /* set the return address after the remap */
	LDR sp, =__cs3_stack
	/* copy vector table to address 0 */
	BL low_level_init

    MSR     CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
    LDR     sp,=__svc_stack_top__              /* set the SVC stack pointer */

	/* get Program Status Register */
	MRS r0, cpsr
	/* go in IRQ mode */
	BIC r1, r0, #0x1F
	ORR r1, r1, #0x12
	MSR cpsr, r1
	/* set IRQ stack */
	LDR sp, =__irq_stack_top__
	/* Enable IRQs */
	BIC r0, r0, #0x80
	/* go back in Supervisor mode */
	MSR cpsr, r0
    /* add peripherals and memory initialization here */
    MSR     CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
    LDR     sp,=__cs3_stack                  /* set the C stack pointer */

    LDR r0, =__cs3_start_asm  
    BX  r0

__cs3_start_asm:  
    LDR r0, =__cs3_start_c  
    BX  r0 

/*****************************************************************************
* void EnterUserMode(void);
*/
    .global EnterUserMode
    .func   EnterUserMode
EnterUserMode:
	/* get Program Status Register */
	MRS r0, cpsr
	/* Change to user mode */
	BIC r1, r0, #0x1F
	ORR r1, r1, #0x10
	MSR cpsr, r1
	MOV	pc, lr
    .endfunc

/*****************************************************************************
* void SystemCall(void);
*/
    .global SystemCall
    .func   SystemCall
SystemCall:
	SWI 0x8;
	MOV	pc, lr
    .endfunc

    .end
