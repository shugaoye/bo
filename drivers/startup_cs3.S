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

__cs3_reset:  
    /* add peripherals and memory initialization here */
    LDR r0, =__cs3_start_asm  
    MOV lr,pc           /* set the return address */
    BX  r0

__cs3_start_asm:  
    /* Set up stack and call C initialization */
    LDR sp, =__cs3_stack
    LDR r0, =__cs3_start_c  
    MOV lr,pc           /* set the return address */
    BX  r0 
    .end
