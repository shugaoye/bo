/*****************************************************************************
* Product: Generic ARM exception code with GNU toolset
* Date of the Last Update:  Jun 27, 2007
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2007 Quantum Leaps, LLC. All rights reserved.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/

    .equ    NO_IRQ,         0x80                     /* mask to disable IRQ */
    .equ    NO_FIQ,         0x40                     /* mask to disable FIQ */
    .equ    NO_INT,         (NO_IRQ | NO_FIQ) /*mask to disable IRQ and FIQ */
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SYS_MODE,       0x1F

    .text
    .code 32

    /* use the special section (.text.fastcode), to enable fine-tuning
    * of the placement of this section inside the linker script
    */
    .section .text.fastcode

/*****************************************************************************
* uint32_t ARM_int_lock_SYS(void);
*/
    .global ARM_int_lock_SYS
    .func   ARM_int_lock_SYS
ARM_int_lock_SYS:
    MRS     r0,cpsr             /* get the original CPSR in r0 to return */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* disable both IRQ/FIQ */
    BX      lr                  /* return the original CPSR in r0 */

    .size   ARM_int_lock_SYS, . - ARM_int_lock_SYS
    .endfunc


/*****************************************************************************
* void ARM_int_unlock_SYS(uint32_t key);
*/
    .global ARM_int_unlock_SYS
    .func   ARM_int_unlock_SYS
ARM_int_unlock_SYS:
    MSR     cpsr_c,r0           /* restore the original CPSR from r0 */
    BX      lr                  /* return to ARM or THUMB */

    .size   ARM_int_unlock_SYS, . - ARM_int_unlock_SYS
    .endfunc


/*****************************************************************************
* void ARM_irq(void);
*/
    .global ARM_irq
    .func   ARM_irq
ARM_irq:
/* IRQ entry {{{ */
    MOV     r13,r0              /* save r0 in r13_IRQ */
    SUB     r0,lr,#4            /* put return address in r0_SYS */
    MOV     lr,r1               /* save r1 in r14_IRQ (lr) */
    MRS     r1,spsr             /* put the SPSR in r1_SYS */

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM, no IRQ, but FIQ enabled! */
    STMFD   sp!,{r0,r1}         /* save SPSR and PC on SYS stack */
    STMFD   sp!,{r2-r3,r12,lr}  /* save APCS-clobbered regs on SYS stack */
    MOV     r0,sp               /* make the sp_SYS visible to IRQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, r1_SYS) */

    MSR     cpsr_c,#(IRQ_MODE | NO_IRQ) /* IRQ mode, IRQ/FIQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/

    MSR     cpsr_c,#(SYS_MODE | NO_IRQ) /* SYSTEM mode, IRQ disabled */
/* IRQ entry }}} */

    /* NOTE: BSP_irq might re-enable IRQ interrupts (the FIQ is enabled
    * already), if IRQs are prioritized by an interrupt controller.
    */
    LDR     r12,=BSP_irq
    MOV     lr,pc               /* copy the return address to link register */
    BX      r12                 /* call the C IRQ-handler (ARM/THUMB) */


/* IRQ exit {{{ */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    MOV     r0,sp               /* make sp_SYS visible to IRQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

    MSR     cpsr_c,#(IRQ_MODE | NO_INT) /* IRQ mode, both IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_IRQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_IRQ */

    LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
    NOP                         /* can't access banked reg immediately */
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR */
/* IRQ exit }}} */

    .size   ARM_irq, . - ARM_irq
    .endfunc


/*****************************************************************************
* void ARM_fiq(void);
*/
    .global ARM_fiq
    .func   ARM_fiq
ARM_fiq:
/* FIQ entry {{{ */
    MOV     r13,r0              /* save r0 in r13_FIQ */
    SUB     r0,lr,#4            /* put return address in r0_SYS */
    MOV     lr,r1               /* save r1 in r14_FIQ (lr) */
    MRS     r1,spsr             /* put the SPSR in r1_SYS */

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    STMFD   sp!,{r0,r1}         /* save SPSR and PC on SYS stack */
    STMFD   sp!,{r2-r3,r12,lr}  /* save APCS-clobbered regs on SYS stack */
    MOV     r0,sp               /* make the sp_SYS visible to FIQ mode */
    SUB     sp,sp,#(2*4)        /* make room for stacking (r0_SYS, SPSR) */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    STMFD   r0!,{r13,r14}       /* finish saving the context (r0_SYS,r1_SYS)*/

    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
/* FIQ entry }}} */

    /* NOTE: NOTE: BSP_fiq must NEVER enable IRQ/FIQ interrrupts!
    */
    LDR     r12,=BSP_fiq
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the C FIQ-handler (ARM/THUMB)


/* FIQ exit {{{ */              /* both IRQ/FIQ disabled (see NOTE above) */
    MOV     r0,sp               /* make sp_SYS visible to FIQ mode */
    ADD     sp,sp,#(8*4)        /* fake unstacking 8 registers from sp_SYS */

    MSR     cpsr_c,#(FIQ_MODE | NO_INT) /* FIQ mode, IRQ/FIQ disabled */
    MOV     sp,r0               /* copy sp_SYS to sp_FIQ */
    LDR     r0,[sp,#(7*4)]      /* load the saved SPSR from the stack */
    MSR     spsr_cxsf,r0        /* copy it into spsr_FIQ */

    LDMFD   sp,{r0-r3,r12,lr}^  /* unstack all saved USER/SYSTEM registers */
    NOP                         /* can't access banked reg immediately */
    LDR     lr,[sp,#(6*4)]      /* load return address from the SYS stack */
    MOVS    pc,lr               /* return restoring CPSR from SPSR */
/* FIQ exit }}} */

    .size   ARM_fiq, . - ARM_fiq
    .endfunc


/*****************************************************************************
* void ARM_reset(void);
*/
    .global ARM_reset
    .func   ARM_reset
ARM_reset:
    LDR     r0,Csting_reset
    B       ARM_except
    .size   ARM_reset, . - ARM_reset
    .endfunc

/*****************************************************************************
* void ARM_undef(void);
*/
    .global ARM_undef
    .func   ARM_undef
ARM_undef:
    LDR     r0,Csting_undef
    B       ARM_except
    .size   ARM_undef, . - ARM_undef
    .endfunc

/*****************************************************************************
* void ARM_swi(void);
*/
    .global ARM_swi
    .func   ARM_swi
ARM_swi:
    STMFD sp!,{r0-r12,lr}	/* Store registers. */
    LDR r0,[lr,#-4]			/* Calculate address of SWI instruction and load it into r0. */

    BIC r0,r0,#0xff000000	/* Mask off top 8 bits of instruction to give SWI number. */
    BL sw_handler

    LDMFD sp!, {r0-r12,pc}^	/* Restore registers and return */
    .size   ARM_swi, . - ARM_swi
    .endfunc

/*****************************************************************************
* void ARM_pAbort(void);
*/
    .global ARM_pAbort
    .func   ARM_pAbort
ARM_pAbort:
    LDR     r0,Csting_pAbort
    B       ARM_except
    .size   ARM_pAbort, . - ARM_pAbort
    .endfunc

/*****************************************************************************
* void ARM_dAbort(void);
*/
    .global ARM_dAbort
    .func   ARM_dAbort
ARM_dAbort:
    LDR     r0,Csting_dAbort
    B       ARM_except
    .size   ARM_dAbort, . - ARM_dAbort
    .endfunc

/*****************************************************************************
* void ARM_reserved(void);
*/
    .global ARM_reserved
    .func   ARM_reserved
ARM_reserved:
    LDR     r0,Csting_rsrvd
    B       ARM_except
    .size   ARM_reserved, . - ARM_reserved
    .endfunc

/*****************************************************************************
* void ARM_except(void);
*/
    .global ARM_except
    .func   ARM_except
ARM_except:
    SUB     r1,lr,#4            /* set line number to the exception address */
    MSR     cpsr_c,#(SYS_MODE | NO_INT) /* SYSTEM mode, IRQ/FIQ disabled */
    LDR     r12,=BSP_abort
    MOV     lr,pc               /* store the return address */
    BX      r12                 /* call the assertion-handler (ARM/THUMB) */
    /* the assertion handler should not return, but in case it does
    * hang up the machine in the following endless loop
    */
    B       .

Csting_reset:  .string  "Reset"
Csting_undef:  .string  "Undefined"
Csting_swi:    .string  "Software Int"
Csting_pAbort: .string  "Prefetch Abort"
Csting_dAbort: .string  "Data Abort"
Csting_rsrvd:  .string  "Reserved Exception"

    .size   ARM_except, . - ARM_except
    .endfunc

    .end
