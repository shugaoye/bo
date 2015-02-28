/******************************************************************************
 *
 * U-Boot support for Goldfish MMC
 *
 * Copyright (c) 2015 Roger Ye.  All rights reserved.
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
#include <asm/io.h>
#include <mmc.h>
#include <configs/goldfish.h>
#else
#include <hardware.h>
#include <bsp.h>
#include <div64.h>
#include "mmc.h"
#endif /* __BARE_METAL__ */

#define DRIVER_NAME "goldfish_mmc"

#define BUFFER_SIZE   16384

#define GOLDFISH_MMC_READ(host, addr)   (readl(host->reg_base + addr))
#define GOLDFISH_MMC_WRITE(host, addr, x)   (writel(x, host->reg_base + addr))


enum {
	/* status register */
	MMC_INT_STATUS	        = 0x00,
	/* set this to enable IRQ */
	MMC_INT_ENABLE	        = 0x04,
	/* set this to specify buffer address */
	MMC_SET_BUFFER          = 0x08,

	/* MMC command number */
	MMC_CMD	                = 0x0C,

	/* MMC argument */
	MMC_ARG	                = 0x10,

	/* MMC response (or R2 bits 0 - 31) */
	MMC_RESP_0		        = 0x14,

	/* MMC R2 response bits 32 - 63 */
	MMC_RESP_1		        = 0x18,

	/* MMC R2 response bits 64 - 95 */
	MMC_RESP_2		        = 0x1C,

	/* MMC R2 response bits 96 - 127 */
	MMC_RESP_3		        = 0x20,

	MMC_BLOCK_LENGTH        = 0x24,
	MMC_BLOCK_COUNT         = 0x28,

	/* MMC state flags */
	MMC_STATE               = 0x2C,

	/* MMC_INT_STATUS bits */

	MMC_STAT_END_OF_CMD     = 1U << 0,
	MMC_STAT_END_OF_DATA    = 1U << 1,
	MMC_STAT_STATE_CHANGE   = 1U << 2,
	MMC_STAT_CMD_TIMEOUT    = 1U << 3,

	/* MMC_STATE bits */
	MMC_STATE_INSERTED      = 1U << 0,
	MMC_STATE_READ_ONLY     = 1U << 1,
};

/*
 * Command types
 */
#define OMAP_MMC_CMDTYPE_BC	0
#define OMAP_MMC_CMDTYPE_BCR	1
#define OMAP_MMC_CMDTYPE_AC	2
#define OMAP_MMC_CMDTYPE_ADTC	3

struct goldfish_mmc_host {
	struct mmc_data *		data;
#if 0
	struct mmc_request *	mrq;
	struct mmc_command *	cmd;
	struct mmc_host *		mmc;
	struct device *			dev;
#endif
	unsigned char			id; /* 16xx chips have 2 MMC blocks */
	void __iomem			*virt_base;
	unsigned int			phys_base;
	int						irq;
	unsigned char			bus_mode;
	unsigned char			hw_bus_mode;

	unsigned int			sg_len;
	unsigned				dma_done:1;
	unsigned				dma_in_use:1;
	void __iomem			*reg_base;
};

/* Global variables for MMC support */
struct mmc g_mmc_dev;
struct goldfish_mmc_host g_mmc_host;

#define MMC_CMD_MASK	(3 << 5)		/* non-SPI command type */
#define MMC_CMD_AC		(0 << 5)
#define MMC_CMD_ADTC	(1 << 5)
#define MMC_CMD_BC		(2 << 5)
#define MMC_CMD_BCR		(3 << 5)
#define mmc_resp_type(cmd)	((cmd)->resp_type & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))
#define mmc_cmd_type(cmd)	((cmd)->resp_type & MMC_CMD_MASK)

static void mmc_prepare_data(struct goldfish_mmc_host *host, struct mmc_data *data)
{
	int block_size;
	unsigned sg_len;

	debug(" mmc_prepare_data called\n");

	host->data = data;

	if (data == NULL) {
		GOLDFISH_MMC_WRITE(host, MMC_BLOCK_LENGTH, 0);
		GOLDFISH_MMC_WRITE(host, MMC_BLOCK_COUNT, 0);
		host->dma_in_use = 0;
		return;
	}

	block_size = data->blocksize;

	GOLDFISH_MMC_WRITE(host, MMC_BLOCK_COUNT, data->blocks - 1);
	GOLDFISH_MMC_WRITE(host, MMC_BLOCK_LENGTH, block_size - 1);

	host->dma_done = 0;
	host->dma_in_use = 1;

}

static int mmc_send_cmd(struct mmc *my_mmc_dev, struct mmc_cmd *cmd,
			struct mmc_data *data)
{
	struct goldfish_mmc_host *host = (struct goldfish_mmc_host *)my_mmc_dev->priv;
	int result;

	uint cmdreg;
	uint resptype;
	uint cmdtype;


	debug(" mmc_send_cmd called\n");

	resptype = 0;
	cmdtype = 0;


	if (result < 0)
		return result;

	if (data)
		mmc_prepare_data(host, data);

	/* Our hardware needs to know exact type */
	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_NONE:
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1b:
		/* resp 1, 1b, 6, 7 */
		resptype = 1;
		break;
	case MMC_RSP_R2:
		resptype = 2;
		break;
	case MMC_RSP_R3:
		resptype = 3;
		break;
	default:
		debug("mmc_send_cmd: Invalid response type: %04x\n", mmc_resp_type(cmd));
		break;
	}

	if (mmc_cmd_type(cmd) == MMC_CMD_ADTC) {
		cmdtype = OMAP_MMC_CMDTYPE_ADTC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BC) {
		cmdtype = OMAP_MMC_CMDTYPE_BC;
	} else if (mmc_cmd_type(cmd) == MMC_CMD_BCR) {
		cmdtype = OMAP_MMC_CMDTYPE_BCR;
	} else {
		cmdtype = OMAP_MMC_CMDTYPE_AC;
	}

	cmdreg = cmd->cmdidx | (resptype << 8) | (cmdtype << 12);

	/* if (host->bus_mode == MMC_BUSMODE_OPENDRAIN)
		cmdreg |= 1 << 6; */

	if (cmd->resp_type & MMC_RSP_BUSY)
		cmdreg |= 1 << 11;

	if (data && !(data->flags & MMC_DATA_WRITE))
		cmdreg |= 1 << 15;

	GOLDFISH_MMC_WRITE(host, MMC_ARG, cmd->cmdarg);
	GOLDFISH_MMC_WRITE(host, MMC_CMD, cmdreg);

	debug("cmd->cmdarg: %08x\n", cmd->cmdarg);

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY)) {
		debug("mmc_send_cmd err. resp_type=%d", cmd->resp_type);
		return -1;
	}

	/* data->src = host->phys_base; */

#ifndef __BARE_METAL__
	udelay(1000);
#else
	__udelay(1000);
#endif /* __BARE_METAL__ */

	return 0;
}

static void mmc_set_ios(struct mmc *my_mmc_dev)
{
	struct goldfish_mmc_host *host = my_mmc_dev->priv;

	debug(" mmc_set_ios called\n");

	debug("bus_width: %x, clock: %d\n", my_mmc_dev->bus_width, my_mmc_dev->clock);
}

static int goldfish_mmc_getcd(struct mmc *my_mmc_dev)
{
	struct goldfish_mmc_host *host = (struct goldfish_mmc_host *)my_mmc_dev->priv;

	debug("goldfish_mmc_getcd called\n");

	return 1;
}

static int mmc_core_init(struct mmc *my_mmc_dev)
{
	struct goldfish_mmc_host *host = (struct goldfish_mmc_host *)my_mmc_dev->priv;
	unsigned int mask;
	debug(" mmc_core_init called\n");

	return 0;
}

/* this is a weak define that we are overriding */
#ifndef __BARE_METAL__
int board_mmc_init(bd_t *bd)
#else
int board_mmc_init()
#endif /* __BARE_METAL__ */
{
	struct goldfish_mmc_host *host = &g_mmc_host;
	struct mmc *my_mmc_dev = &g_mmc_dev;
	unsigned int buf_addr;

	debug("board_mmc_init called\n");

	buf_addr = (void *)malloc(BUFFER_SIZE);

	sprintf(my_mmc_dev->name, DRIVER_NAME);
	my_mmc_dev->priv = host;
	my_mmc_dev->send_cmd = mmc_send_cmd;
	my_mmc_dev->set_ios = mmc_set_ios;
	my_mmc_dev->init = mmc_core_init;
	my_mmc_dev->getcd = goldfish_mmc_getcd;
	my_mmc_dev->f_min = 400000;
	my_mmc_dev->f_max = 24000000;
	my_mmc_dev->host_caps = MMC_MODE_4BIT;

	host->reg_base = (void *)IO_ADDRESS(GOLDFISH_MMC_BASE);
	host->phys_base = buf_addr;
	debug("board_mmc_init: host->phys_base=%x\n", host->phys_base);


	GOLDFISH_MMC_WRITE(host, MMC_SET_BUFFER, buf_addr);
	GOLDFISH_MMC_WRITE(host, MMC_INT_ENABLE,
		MMC_STAT_END_OF_CMD | MMC_STAT_END_OF_DATA | MMC_STAT_STATE_CHANGE |
		MMC_STAT_CMD_TIMEOUT);

#ifndef __BARE_METAL__
	mmc_register(my_mmc_dev);
#endif /* __BARE_METAL__ */

	return 0;
}
