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

    .global __cs3_reset 
	.global vectors_start
	.global vectors_end

vectors_start:
	LDR PC, reset_handler_addr
	LDR PC, undef_handler_addr
	LDR PC, swi_handler_addr
	LDR PC, prefetch_abort_handler_addr
	LDR PC, data_abort_handler_addr
	B .
	LDR PC, irq_handler_addr
	LDR PC, fiq_handler_addr

reset_handler_addr: .word __cs3_reset
undef_handler_addr: .word ARM_undef
swi_handler_addr: .word ARM_swi
prefetch_abort_handler_addr: .word ARM_pAbort
data_abort_handler_addr: .word ARM_dAbort
irq_handler_addr: .word ARM_irq
fiq_handler_addr: .word ARM_fiq

vectors_end:

__cs3_reset:  
	/* set Supervisor stack */
    LDR     r0,=__cs3_reset      /* pass the reset address as the 1st argument */
    LDR     r1,=__cs3_start_asm   /* pass the return address as the 2nd argument */
    MOV     lr,r1           /* set the return address after the remap */
	LDR sp, =__cs3_stack
	/* copy vector table */
    LDR     r0,=0
    LDR     r1,=vectors_start
    LDR     r2,=vectors_end
1:
    CMP     r1,r2
    LDMLTIA r1!,{r3}
    STMLTIA r0!,{r3}
    BLT     1b

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
    LDR r0, =__cs3_start_asm  
    BX  r0

__cs3_start_asm:  
    LDR r0, =__cs3_start_c  
    BX  r0 
    .end
