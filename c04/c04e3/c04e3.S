/******************************************************************************
 *
 * c04e3.S - demonstrate a simple add of two number in RAM
 *
 * Copyright (c) 2012 Roger Ye.  All rights reserved.
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
	.syntax unified
	.cpu arm7tdmi
	.fpu softvfp

/******************************************************************************
 *
 * Variables allocated in RAM
 *
*******************************************************************************/
	.data
val1:	.4byte 10       @ First number
val2:	.4byte 30       @ Second number
result:	.space 4        @ 1 byte space for result	

/******************************************************************************
 *
 * This is the code that gets called when the processor first starts execution
 * following a reset event.
 *
*******************************************************************************/
	.section	.text.ResetISR,"ax",%progbits
	.align	2
	.global	ResetISR
	.type	ResetISR, %function
ResetISR:

    /* Add and store result. */
    ldr   r0, =val1     @ r0 = &val1
    ldr   r1, =val2     @ r1 = &val2

    ldr   r2, [r0]      @ r2 = *r0
    ldr   r3, [r1]      @ r3 = *r1

    add   r4, r2, r3    @ r4 = r2 + r3

    ldr   r0, =result   @ r0 = &result
    str   r4, [r0]      @ *r0 = r4
ResetISR_STOP:
    b ResetISR_STOP     @ Infinite loop to stop execution
    .size	ResetISR, .-ResetISR

