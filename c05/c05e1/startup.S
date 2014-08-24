/******************************************************************************
 *
 * startup.S - C startup code using assembly
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

/* constant to pre-fill the stack */
    .equ    STACK_FILL,     0xAAAAAAAA

	.arch armv5te
	.fpu softvfp

	.global	version
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Copyright (c) 2013, Roger Ye. All rights reserved.\012"
	.ascii	"\000"

	.data
	.align	2
	.type	version, %object
	.size	version, 4
version:
	.word	.LC0

/******************************************************************************
 *
 * This is the code that gets called when the processor first starts execution
 * following a reset event.
 *
*******************************************************************************/
	.section	.text.ResetISR,"ax",%progbits
	.align	2
	.global	 ResetISR
	.type	ResetISR, %function
ResetISR:
	@@ Copy data to RAM.
	ldr   r0, =_etext
	ldr   r1, =_data
	ldr   r2, =_edata
	sub   r2, r2, r1

	@@ Handle data_size == 0
	cmp   r2, #0
	beq   init_bss
copy:
	ldrb   r4, [r0], #1
	strb   r4, [r1], #1
	subs   r2, r2, #1
	bne    copy

init_bss:
	@@ Initialize .bss
	ldr   r0, =_bss
	ldr   r1, =_ebss
	sub   r2, r1, r0

	@@ Handle bss_size == 0
	cmp   r2, #0
	beq   init_stack
	
	mov   r4, #0
zero:	
	strb  r4, [r0], #1
	subs  r2, r2, #1
	bne   zero

init_stack:
	@@ Initialize the stack pointer
    /* Fill the .stack section */
    LDR     r1,=__stack_start__
    LDR     r2,=__stack_end__
    LDR     r3,=STACK_FILL
1:
    CMP     r1,r2
    STMLTIA r1!,{r3}
    BLT     1b

    /* Initialize stack pointers for all ARM modes */
    MSR     CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__irq_stack_top__              /* set the IRQ stack pointer */

    MSR     CPSR_c,#(FIQ_MODE | I_BIT | F_BIT)
    LDR     sp,=__fiq_stack_top__              /* set the FIQ stack pointer */

    MSR     CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
    LDR     sp,=__svc_stack_top__              /* set the SVC stack pointer */

    MSR     CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
    LDR     sp,=__abt_stack_top__              /* set the ABT stack pointer */

    MSR     CPSR_c,#(UND_MODE | I_BIT | F_BIT)
    LDR     sp,=__und_stack_top__              /* set the UND stack pointer */

    MSR     CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
    LDR     sp,=__c_stack_top__                  /* set the C stack pointer */

call_main:
	mov    r0, #1
	ldr    r1, =version
	bl    main

ResetISR_STOP:
	b	ResetISR_STOP    @ Infinite loop to stop execution
	.size	ResetISR, .-ResetISR

