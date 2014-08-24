/*****************************************************************************
* Product: Generic startup code for ARM with GNU toolset
* Date of the Last Update:  Jan 13, 2010
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2007-2010 Quantum Leaps, LLC. All rights reserved.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
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

/*****************************************************************************
* The startup code must be linked at the start of ROM, which is NOT
* necessarily address zero.
*/
    .text
    .code 32

    .global _vectors
    .func   _vectors

_vectors:

    /* Vector table
    * NOTE: used only very briefly until RAM is remapped to address zero
    */
    B       _reset          /* Reset: relative branch allows remap */
    B       .               /* Undefined Instruction */
    B       .               /* Software Interrupt    */
    B       .               /* Prefetch Abort        */
    B       .               /* Data Abort            */
    B       .               /* Reserved              */
    B       .               /* IRQ                   */
    B       .               /* FIQ                   */

    .size   _vectors, . - _vectors
    .endfunc

/******************************************************************************
 *
 * The copyright notice in the ROM
 *
*******************************************************************************/
    .global version
    .section    .rodata
    .align  2
.LC0:
    .ascii  "Copyright (c) 2013, Roger Ye. All rights reserved.\012"
    .ascii  "\000"

    .data
    .align  4
    .type   version, %object
    .size   version, 4
version:
    .word   .LC0

    .align 4                               /* re-align to the word boundary */

/*****************************************************************************
* _reset
*/
    .text
    .code 32
    .func   _reset

_reset:

    /* Call the platform-specific low-level initialization routine
    *
    * NOTE: The ROM is typically NOT at its linked address before the remap,
    * so the branch to low_level_init() must be relative (position
    * independent code). The low_level_init() function must continue to
    * execute in ARM state. Also, the function low_level_init() cannot rely
    * on uninitialized data being cleared and cannot use any initialized
    * data, because the .bss and .data sections have not been initialized yet.
    */
    LDR     r0,=_reset      /* pass the reset address as the 1st argument */
    LDR     r1,=_cstartup   /* pass the return address as the 2nd argument */
    MOV     lr,r1           /* set the return address after the remap */
    LDR     sp,=__stack_end__  /* set the temporary stack pointer */
    B       low_level_init  /* relative branch enables remap */


    /* NOTE: after the return from low_level_init() the ROM is remapped
    * to its linked address so the rest of the code executes at its linked
    * address.
    */

_cstartup:
    /* Relocate .fastcode section (copy from ROM to RAM) */
    LDR     r0,=__fastcode_load
    LDR     r1,=__fastcode_start
    LDR     r2,=__fastcode_end
1:
    CMP     r1,r2
    LDMLTIA r0!,{r3}
    STMLTIA r1!,{r3}
    BLT     1b


    /* Relocate the .data section (copy from ROM to RAM) */
    LDR     r0,=__data_load
    LDR     r1,=__data_start
    LDR     r2,=_edata
1:
    CMP     r1,r2
    LDMLTIA r0!,{r3}
    STMLTIA r1!,{r3}
    BLT     1b


    /* Clear the .bss section (zero init) */
    LDR     r1,=__bss_start__
    LDR     r2,=__bss_end__
    MOV     r3,#0
1:
    CMP     r1,r2
    STMLTIA r1!,{r3}
    BLT     1b


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
#if 0
    /* Invoke the static C++ constructors (harmless in C) */
    LDR     r12,=__libc_init_array
    MOV     lr,pc           /* set the return address */
    BX      r12             /* the target code can be ARM or THUMB */
#endif
    /* Enter the C/C++ code */
    MOV     r0, #1
    LDR     r1, =version
    LDR     r12,=main
    MOV     lr,pc           /* set the return address */
    BX      r12             /* the target code can be ARM or THUMB */

    SWI     0xFFFFFF        /* cause exception if main() ever returns */

    .size   _reset, . - _reset
    .endfunc

    .end
