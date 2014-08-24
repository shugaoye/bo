#******************************************************************************
#
# Makefile - Makefile of example c07e1
#
# Copyright (c) 2013 Roger Ye.  All rights reserved.
# Software License Agreement
# 
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. The AUTHOR SHALL NOT, UNDER
# ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
#******************************************************************************

# This is the makefile for goldfish serial device testing

#
# The base directory.
#
ROOT=../..
PROJECTNAME=c07e2

#
# Include the common make definitions.
#
include ${ROOT}/makedefs.arm

CFLAGS+=-D__BARE_METAL__ -D__USE_CLIB__
AFLAGS+=-D__BARE_METAL__

LDFLAGS+=-L${TOOLROOT}/arm-none-eabi/lib -L${TOOLROOT}/lib/gcc/arm-none-eabi/4.6.3

#
# Where to find source files that do not live in this directory.
#
VPATH=../../drivers

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=../../include

#
# The default rule, which causes the ${PROJECTNAME} example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/${PROJECTNAME}.axf

#
# The rule to run the target using Android emulator.
#
run:
	emulator -verbose -show-kernel -netfast -avd hd2 -shell -qemu -monitor telnet::6666,server -kernel ${COMPILER}/${PROJECTNAME}.axf

#
# The rule to debug the target using Android emulator.
# This make target uses bin file to simulate a rom image
#
debug:
	ddd --debugger arm-none-eabi-gdb ${COMPILER}/${PROJECTNAME}.axf &
	emulator -verbose -show-kernel -netfast -avd hd2 -shell -qemu -monitor telnet::6666,server -s -S -kernel ${COMPILER}/${PROJECTNAME}.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the ${PROJECTNAME} example.
#
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/${PROJECTNAME}.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/startup_${PROJECTNAME}.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/serial_goldfish.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/goldfish_uart.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/syscalls_cs3.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/low_level_init.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/arm_exc.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/bsp.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/isr.o
${COMPILER}/${PROJECTNAME}.axf: ${COMPILER}/timer.o
${COMPILER}/${PROJECTNAME}.axf: ${PROJECTNAME}.ld
SCATTERgcc_${PROJECTNAME}=${PROJECTNAME}.ld
ENTRY_${PROJECTNAME}=__cs3_reset

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
