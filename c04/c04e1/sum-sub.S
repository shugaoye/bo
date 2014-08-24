/******************************************************************************
 *
 * sum-sub.s - demonstrate a simple sum of two number using assembly
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

    .section .data
data2:
    .word 0

/******************************************************************************
 *
 * This is a function to sum an array of integer.
 * Input Parameters:
 * r0: Start address of array
 * r1: End address of array
 *
 * Result
 * r3: Sum of Array
 *
*******************************************************************************/

    .section .text.sum
    .align
    .global sum
	
sum:    
    mov   r3, #0          @ r3 = 0
loop:   
    ldrb  r2, [r0], #1    @ r2 = *r0++      ; Get array element
    add   r3, r2, r3      @ r3 += r2        ; Calculate sum
    cmp   r0, r1          @ if (r0 != r1)   ; Check if hit end-of-array
    bne   loop            @ goto loop       ; Loop
    ldr   r0, =data2      @ r0 = &data2
    str   r3, [r0]        @ result in r3 and store it to data1
    mov   pc, lr          @ pc = lr         ; Return when done
