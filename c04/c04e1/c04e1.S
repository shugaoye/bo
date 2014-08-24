/******************************************************************************
 *
 * c04e1.S - demonstrate a simple sum of an integer array using assembly
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

    .section  .data
data1:
    .word	0

/******************************************************************************
 *
 * This is the code that gets called when the processor first starts execution
 * following a reset event.
 *
*******************************************************************************/
    .section .text.ResetISR
    .align
    .global ResetISR
    .type ResetISR, %function
ResetISR:
    b start                 @ Skip over the data
arr:
    .byte 2, 4, 8, 16, 32   @ Read-only array of bytes
eoa:                        @ Address of end of array + 1

    .align
start:
    ldr   r0, =arr          @ r0 = &arr
    ldr   r1, =eoa          @ r1 = &eoa

    bl    sum               @ Invoke the sum subroutine
    ldr   r0, =data1        @ r0 = &data1
    str   r3, [r0]          @ result in r3 and store it to data1
ResetISR_STOP:
    b ResetISR_STOP    @ Infinite loop to stop execution
    .size ResetISR, .-ResetISR

