#!/bin/bash
#******************************************************************************
#
# Shell Script used to setup the SDK environment for emulator
#
# Copyright (c) 2014 Roger Ye.  All rights reserved.
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
if [ -f ./bin/emulator ]; then
	echo "Find the emulator."
else
	echo "Cannot find emulator. Downloading ..."
	wget -t1 -O ./emulator_arm.tar.gz "http://downloads.sourceforge.net/project/epwa/emulator_arm.tar.gz?r=&ts=1427204475&use_mirror=master"
	tar xvfz emulator_arm.tar.gz
	rm emulator_arm.tar.gz
fi

if [ -x ${AndroidSDK}/platforms/android-15 ]; then
        echo "Find API level 15."
	if [ -x ./platforms ]; then
		echo "Find platforms"
	else
		ln -s ${AndroidSDK}/platforms .
	fi
	if [ -x ./system-images ]; then
		echo "Find system-images"
	else
		ln -s ${AndroidSDK}/system-images .
	fi
else
        echo "Cannot find API level 15. Please download API level 15 in order to create the version of virtual device needed in this book."
	echo ${AndroidSDK}/platforms/android-15
fi

