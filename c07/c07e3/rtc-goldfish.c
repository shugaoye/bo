/******************************************************************************
 *
 * rtc-goldfish.c - RTC support for U-Boot
 *
 * Copyright (c) 2014 Roger Ye.  All rights reserved.
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
#ifndef __BARE_METAL__
#include <common.h>
#include <command.h>
#include <rtc.h>
#include <asm/io.h>
#include <configs/goldfish.h>
#else
#include <hardware.h>
#include <bsp.h>
#include "rtc.h"
#endif

#if defined(CONFIG_CMD_DATE)

/* This is the offset for the rtc_set(). */
static unsigned long rtc_offset = 0;

int rtc_get(struct rtc_time *tmp)
{
	ulong rv;

	rv = get_second() + rtc_offset;

	to_tm(rv, tmp);

	return 0;
}

int rtc_set(struct rtc_time *tmp)
{
	unsigned long rv;

	rv = mktime (tmp->tm_year, tmp->tm_mon,
			tmp->tm_mday, tmp->tm_hour,
			tmp->tm_min, tmp->tm_sec);

	rtc_offset = rv - get_second();

	return 0;
}

void rtc_reset(void)
{
	rtc_offset = 0;
}
#endif
