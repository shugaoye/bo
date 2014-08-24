/* drivers/mtd/devices/goldfish_nand.c
**
** Copyright (C) 2007 Google, Inc.
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
*/

/******************************************************************************
 *
 * Add U-Boot support
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
#include <div64.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <asm/errno.h>
#include <configs/goldfish.h>
#include <ubi_uboot.h>
#include <nand.h>
#else
#include <hardware.h>
#include <bsp.h>
#include "mtd.h"
#include "div64.h"
#endif /* __BARE_METAL__ */

#include "goldfish_nand_reg.h"

#define GOLDFISH_DEV_NAME_MAX_LEN 64

struct goldfish_nand {
	unsigned char __iomem  *base;
	size_t                  mtd_count;
	struct mtd_info         *mtd;
};

struct goldfish_nand goldfish_nand_info;

#ifdef __BARE_METAL__
typedef struct mtd_info nand_info_t;

nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
#else
extern nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
#endif

static char goldfish_dev_name[CONFIG_SYS_MAX_NAND_DEVICE][GOLDFISH_DEV_NAME_MAX_LEN];

static uint32_t goldfish_nand_cmd(struct mtd_info *mtd, enum nand_cmd cmd,
                              uint64_t addr, uint32_t len, void *ptr)
{
	struct goldfish_nand *nand = mtd->priv;
	uint32_t rv;
	unsigned char __iomem  *base = nand->base;

	writel(mtd - nand->mtd, base + NAND_DEV);
	writel((uint32_t)(addr >> 32), base + NAND_ADDR_HIGH);
	writel((uint32_t)addr, base + NAND_ADDR_LOW);
	writel(len, base + NAND_TRANSFER_SIZE);
	writel((unsigned long)ptr, base + NAND_DATA);
	writel(cmd, base + NAND_COMMAND);
	rv = readl(base + NAND_RESULT);

	return rv;
}

static int goldfish_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	loff_t ofs = instr->addr;
	uint32_t len = instr->len;
	uint32_t rem;

	if (ofs + len > mtd->size)
		goto invalid_arg;
	rem = do_div(ofs, mtd->writesize);
	if(rem)
		goto invalid_arg;
	ofs *= (mtd->writesize + mtd->oobsize);
	
	if(len % mtd->writesize)
		goto invalid_arg;
	len = len / mtd->writesize * (mtd->writesize + mtd->oobsize);

	if(goldfish_nand_cmd(mtd, NAND_CMD_ERASE, ofs, len, NULL) != len) {
		debug("goldfish_nand_erase: erase failed, start %llx, len %x, dev_size "
		       "%llx, erase_size %x\n", ofs, len, mtd->size, mtd->erasesize);
		return -EIO;
	}

	instr->state = MTD_ERASE_DONE;
	mtd_erase_callback(instr);

	return 0;

invalid_arg:
	debug("goldfish_nand_erase: invalid erase, start %llx, len %x, dev_size "
	       "%llx, erase_size %x\n", ofs, len, mtd->size, mtd->erasesize);
	return -EINVAL;
}

static int goldfish_nand_read_oob(struct mtd_info *mtd, loff_t ofs,
                              struct mtd_oob_ops *ops)
{
	uint32_t rem;

	if(ofs + ops->len > mtd->size)
		goto invalid_arg;
	if(ops->datbuf && ops->len && ops->len != mtd->writesize)
		goto invalid_arg;
	if(ops->ooblen + ops->ooboffs > mtd->oobsize)
		goto invalid_arg;

	rem = do_div(ofs, mtd->writesize);
	if(rem)
		goto invalid_arg;
	ofs *= (mtd->writesize + mtd->oobsize);

	if(ops->datbuf)
		ops->retlen = goldfish_nand_cmd(mtd, NAND_CMD_READ, ofs,
		                            ops->len, ops->datbuf);
	ofs += mtd->writesize + ops->ooboffs;
	if(ops->oobbuf)
		ops->oobretlen = goldfish_nand_cmd(mtd, NAND_CMD_READ, ofs,
		                               ops->ooblen, ops->oobbuf);
	return 0;

invalid_arg:
	debug("goldfish_nand_read_oob: invalid read, start %llx, len %x, "
	       "ooblen %x, dev_size %llx, write_size %x\n",
	       ofs, ops->len, ops->ooblen, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_write_oob(struct mtd_info *mtd, loff_t ofs,
                               struct mtd_oob_ops *ops)
{
	uint32_t rem;

	if(ofs + ops->len > mtd->size)
		goto invalid_arg;
	if(ops->len && ops->len != mtd->writesize)
		goto invalid_arg;
	if(ops->ooblen + ops->ooboffs > mtd->oobsize)
		goto invalid_arg;
	
	rem = do_div(ofs, mtd->writesize);
	if(rem)
		goto invalid_arg;
	ofs *= (mtd->writesize + mtd->oobsize);

	if(ops->datbuf)
		ops->retlen = goldfish_nand_cmd(mtd, NAND_CMD_WRITE, ofs,
		                            ops->len, ops->datbuf);
	ofs += mtd->writesize + ops->ooboffs;
	if(ops->oobbuf)
		ops->oobretlen = goldfish_nand_cmd(mtd, NAND_CMD_WRITE, ofs,
		                               ops->ooblen, ops->oobbuf);
	return 0;

invalid_arg:
	debug("goldfish_nand_write_oob: invalid write, start %llx, len %x, "
	       "ooblen %x, dev_size %llx, write_size %x\n",
	       ofs, ops->len, ops->ooblen, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_read(struct mtd_info *mtd, loff_t from, size_t len,
                          size_t *retlen, u_char *buf)
{
	uint32_t rem;

	if(from + len > mtd->size)
		goto invalid_arg;
	if(len != mtd->writesize)
		goto invalid_arg;

	rem = do_div(from, mtd->writesize);
	if(rem)
		goto invalid_arg;
	from *= (mtd->writesize + mtd->oobsize);

	*retlen = goldfish_nand_cmd(mtd, NAND_CMD_READ, from, len, buf);
	return 0;

invalid_arg:
	debug("goldfish_nand_read: invalid read, start %llx, len %x, dev_size %llx"
	       ", write_size %x\n", from, len, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_write(struct mtd_info *mtd, loff_t to, size_t len,
                           size_t *retlen, const u_char *buf)
{
	uint32_t rem;

	if(to + len > mtd->size)
		goto invalid_arg;
	if(len != mtd->writesize)
		goto invalid_arg;

	rem = do_div(to, mtd->writesize);
	if(rem)
		goto invalid_arg;
	to *= (mtd->writesize + mtd->oobsize);

	*retlen = goldfish_nand_cmd(mtd, NAND_CMD_WRITE, to, len, (void *)buf);
	return 0;

invalid_arg:
	debug("goldfish_nand_write: invalid write, start %llx, len %x, dev_size %llx"
	       ", write_size %x\n", to, len, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	uint32_t rem;

	if(ofs >= mtd->size)
		goto invalid_arg;

	rem = do_div(ofs, mtd->erasesize);
	if(rem)
		goto invalid_arg;
	ofs *= mtd->erasesize / mtd->writesize;
	ofs *= (mtd->writesize + mtd->oobsize);

	return goldfish_nand_cmd(mtd, NAND_CMD_BLOCK_BAD_GET, ofs, 0, NULL);

invalid_arg:
	debug("goldfish_nand_block_isbad: invalid arg, ofs %llx, dev_size %llx, "
	       "write_size %x\n", ofs, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	uint32_t rem;

	if(ofs >= mtd->size)
		goto invalid_arg;

	rem = do_div(ofs, mtd->erasesize);
	if(rem)
		goto invalid_arg;
	ofs *= mtd->erasesize / mtd->writesize;
	ofs *= (mtd->writesize + mtd->oobsize);

	if(goldfish_nand_cmd(mtd, NAND_CMD_BLOCK_BAD_SET, ofs, 0, NULL) != 1)
		return -EIO;
	return 0;

invalid_arg:
	debug("goldfish_nand_block_markbad: invalid arg, ofs %llx, dev_size %llx, "
	       "write_size %x\n", ofs, mtd->size, mtd->writesize);
	return -EINVAL;
}

static int goldfish_nand_init_device(struct goldfish_nand *nand, int id)
{
	uint32_t name_len;
	uint32_t result;
	uint32_t flags;
	unsigned char __iomem  *base = nand->base;
	struct mtd_info *mtd = &nand->mtd[id];
	char *name;

	writel(id, base + NAND_DEV);
	flags = readl(base + NAND_DEV_FLAGS);
	name_len = readl(base + NAND_DEV_NAME_LEN);
	mtd->writesize = readl(base + NAND_DEV_PAGE_SIZE);
	mtd->size = readl(base + NAND_DEV_SIZE_LOW);
	mtd->size |= (uint64_t)readl(base + NAND_DEV_SIZE_HIGH) << 32;
	mtd->oobsize = readl(base + NAND_DEV_EXTRA_SIZE);
	mtd->oobavail = mtd->oobsize;
	mtd->erasesize = readl(base + NAND_DEV_ERASE_SIZE) /
	                 (mtd->writesize + mtd->oobsize) * mtd->writesize;
	do_div(mtd->size, mtd->writesize + mtd->oobsize);
	mtd->size *= mtd->writesize;


	mtd->priv = nand;

	if(name_len > GOLDFISH_DEV_NAME_MAX_LEN) {
		debug("goldfish_nand_init_device: name_len=%d, larger than maximum length.", name_len);
		return -ENOMEM;
	}
	mtd->name = name = &goldfish_dev_name[id];

	result = goldfish_nand_cmd(mtd, NAND_CMD_GET_DEV_NAME, 0, name_len, name);
	if(result != name_len) {
		mtd->name = NULL;
		debug("goldfish_nand_init_device failed to get dev name %d != %d\n",
		       result, name_len);
		return -ENODEV;
	}
	((char *) mtd->name)[name_len] = '\0';

	/* Setup the MTD structure */
	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH;
	if(flags & NAND_DEV_FLAG_READ_ONLY)
		mtd->flags &= ~MTD_WRITEABLE;

	mtd->owner = THIS_MODULE;
	mtd->erase = goldfish_nand_erase;
	mtd->read = goldfish_nand_read;
	mtd->write = goldfish_nand_write;
	mtd->read_oob = goldfish_nand_read_oob;
	mtd->write_oob = goldfish_nand_write_oob;
	mtd->block_isbad = goldfish_nand_block_isbad;
	mtd->block_markbad = goldfish_nand_block_markbad;

	return 0;
}

void board_nand_init(void)
{
	uint32_t num_dev;
	int i;
	int err;
	uint32_t num_dev_working;
	uint32_t version;
	struct goldfish_nand *nand;
	unsigned char __iomem  *base = (void *)IO_ADDRESS(GOLDFISH_NAND_BASE);

	debug("base=%x\n", (unsigned int)base);
	version = readl(base + NAND_VERSION);
	if(version != NAND_VERSION_CURRENT) {
		debug("goldfish_nand_init: version mismatch, got %d, expected %d\n",
		       version, NAND_VERSION_CURRENT);
		err = -ENODEV;
		goto err_no_dev;
	}
	num_dev = readl(base + NAND_NUM_DEV);
	if(num_dev == 0 || num_dev > CONFIG_SYS_MAX_NAND_DEVICE) {
		err = -ENODEV;
		debug("goldfish_nand_init: NAND_NUM_DEV=%d, fatal error!", num_dev);
		goto err_no_dev;
	}

	nand = &goldfish_nand_info;

	nand->base = base;
	nand->mtd_count = num_dev;
	nand->mtd = &nand_info[0];

	num_dev_working = 0;
	for(i = 0; i < num_dev; i++) {
		err = goldfish_nand_init_device(nand, i);
		if(err == 0) {
			num_dev_working++;
#ifndef __BARE_METAL__
			nand_register(i);
#endif
			debug("goldfish_nand_init: id=%d: name=%s, nand_name=%s\n",
			       i, nand->mtd[i].name, goldfish_dev_name[i]);

		}
	}
	if(num_dev_working == 0) {
		err = -ENODEV;
		goto err_no_working_dev;
	}
	return;

err_no_working_dev:
err_no_dev:
	return;
}

