/******************************************************************************
 *
 * Linker script for uart
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

OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00010000, LENGTH = 128K
    RAM (rwx)  : ORIGIN = 0x00030000, LENGTH = 512M
}

/* The sizes of the stacks used by the application. NOTE: you need to adjust */
C_STACK_SIZE   = 512;
IRQ_STACK_SIZE = 256;
FIQ_STACK_SIZE = 256;
SVC_STACK_SIZE = 256;
ABT_STACK_SIZE = 256;
UND_STACK_SIZE = 256;

/* The size of the heap used by the application. NOTE: you need to adjust   */
HEAP_SIZE = 0;

SECTIONS
{
  . = 0x10000;
  _text = .;

  .text : {
    KEEP(*(.isr_vector))
    _start = .;
    *(.start)
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
  } > FLASH

  _etext = .;

  .data	: AT(ADDR(.text) + SIZEOF(.text)){
  	  _data = .;
   	  *(.data) 
   } > RAM

  . = ALIGN(4);
  .bss : {
        _edata = .;
        data_size = _edata - _data;
        __bss_start = .;
        _bss = .;
   		*(.bss) 
   } > RAM

  .stack (NOLOAD)	: { 
        _ebss = .;
		bss_size = _ebss - _bss;
          __stack_start__ = . ;

        . += IRQ_STACK_SIZE;
        . = ALIGN (4);
        __irq_stack_top__ = . ;

        . += FIQ_STACK_SIZE;
        . = ALIGN (4);
        __fiq_stack_top__ = . ;

        . += SVC_STACK_SIZE;
        . = ALIGN (4);
        __svc_stack_top__ = . ;

        . += ABT_STACK_SIZE;
        . = ALIGN (4);
        __abt_stack_top__ = . ;

        . += UND_STACK_SIZE;
        . = ALIGN (4);
        __und_stack_top__ = . ;

        . += C_STACK_SIZE;
        . = ALIGN (4);
        __c_stack_top__ = . ;

  		*(.stack) 
  		
        __stack_end__ = .;
  } > RAM
}

