#include <spinand.h>

struct spinand_info_t {
	char * name;
	uint32_t id;
	uint32_t page_size;
	uint32_t spare_size;
	uint32_t pages_per_block;
	uint32_t blocks_per_die;
	uint32_t planes_per_die;
	uint32_t ndies;
};

struct spinand_pdata_t {
	struct spinand_info_t info;
	uint32_t swapbuf;
	uint32_t swaplen;
	uint32_t cmdlen;
};

enum {
	OPCODE_RDID					= 0x9f,
	OPCODE_GET_FEATURE			= 0x0f,
	OPCODE_SET_FEATURE			= 0x1f,
	OPCODE_FEATURE_STATUS		= 0xc0,
	OPCODE_FEATURE_LOCK			= 0xa0,
	OPCODE_READ_PAGE_TO_CACHE	= 0x13,
	OPCODE_READ_PAGE_FROM_CACHE	= 0x03,
	OPCODE_WRITE_ENABLE			= 0x06,
	OPCODE_BLOCK_ERASE			= 0xd8,
	OPCODE_PROGRAM_LOAD			= 0x02,
	OPCODE_PROGRAM_EXECUTE		= 0x10,
	OPCODE_RESET				= 0xff,
};

static const struct spinand_info_t spinand_infos[] = {
	/* Macronix */
	{ "mx35lf2ge4ad",   0xc226, 2048,  64,  64, 2048, 1, 1 },
	{ "mx35lf4ge4ad",   0xc237, 4096, 128,  64, 2048, 1, 1 },

	/* Micron */
	{ "mt29f1g01aaadd", 0x2c12, 2048,  64,  64, 1024, 1, 1 },
	{ "mt29f1g01abafd", 0x2c14, 2048, 128,  64, 1024, 1, 1 },
	{ "mt29f2g01abagd", 0x2c24, 2048, 128,  64, 2048, 2, 1 },
	{ "mt29f4g01abafd", 0x2c34, 4096, 256,  64, 2048, 1, 1 },
	{ "mt29f4g01adagd", 0x2c36, 2048, 128,  64, 2048, 2, 2 },
	{ "mt29f8g01adafd", 0x2c46, 4096, 256,  64, 2048, 1, 2 },
};

static inline int spinand_read_id(struct xfel_ctx_t * ctx, uint32_t swapbuf, uint32_t swaplen, uint32_t cmdlen, uint32_t * id)
{
	uint8_t tx[2];
	uint8_t rx[2];

	tx[0] = OPCODE_RDID;
	tx[1] = 0x0;
	if(!fel_spi_xfer(ctx, swapbuf, swaplen, cmdlen, tx, 2, rx, 2))
		return 0;
	*id = (rx[0] << 8) | (rx[1] << 0);
	return 1;
}

static inline int spinand_info(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
	const struct spinand_info_t * t;
	uint32_t id;
	int i;

	if(spinand_read_id(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, &id) && (id != 0xffffff) && (id != 0))
	{
		for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
		{
			t = &spinand_infos[i];
			if(id == t->id)
			{
				memcpy(&pdat->info, t, sizeof(struct spinand_info_t));
				return 1;
			}
		}
		printf("The spi nand flash '0x%x' is not yet supported\r\n", id);
	}
	return 0;
}

static inline int spinand_reset(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
	uint8_t tx[1];

	tx[0] = OPCODE_RESET;
	if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 1, 0, 0))
		return 0;
	usleep(10 * 1000);
	return 1;
}

static inline int spinand_get_feature(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint8_t addr, uint8_t * val)
{
	uint8_t tx[2];

	tx[0] = OPCODE_GET_FEATURE;
	tx[1] = addr;
	if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 2, val, 1))
		return 0;
	return 1;
}

static inline int spinand_set_feature(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint8_t addr, uint8_t val)
{
	uint8_t tx[3];

	tx[0] = OPCODE_SET_FEATURE;
	tx[1] = addr;
	tx[2] = val;
	if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 3, 0, 0))
		return 0;
	return 1;
}

static inline int spinand_wait_for_busy(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	cbuf[clen++] = SPI_CMD_SELECT;
	cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
	cbuf[clen++] = SPI_CMD_DESELECT;
	cbuf[clen++] = SPI_CMD_END;
	if(clen <= pdat->cmdlen)
	{
		fel_chip_spi_run(ctx, cbuf, clen);
		return 1;
	}
	return 0;
}

static int spinand_helper_init(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
	uint8_t val;

	if(fel_spi_init(ctx, &pdat->swapbuf, &pdat->swaplen, &pdat->cmdlen) && spinand_info(ctx, pdat))
	{
		spinand_reset(ctx, pdat);
		spinand_wait_for_busy(ctx, pdat);
		if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_LOCK, &val) && (val & 0x38))
			spinand_set_feature(ctx, pdat, OPCODE_FEATURE_LOCK, val & ~0x38);
		return 1;
	}
	return 0;
}

static void spinand_helper_read(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint32_t addr, uint8_t * buf, uint32_t count)
{
	uint32_t pa, ca;
	uint32_t n;
	uint8_t tx[4];

	while(count > 0)
	{
		pa = addr / pdat->info.page_size;
		ca = addr & (pdat->info.page_size - 1);
		n = count > (pdat->info.page_size - ca) ? (pdat->info.page_size - ca) : count;
		tx[0] = OPCODE_READ_PAGE_TO_CACHE;
		tx[1] = (uint8_t)(pa >> 16);
		tx[2] = (uint8_t)(pa >> 8);
		tx[3] = (uint8_t)(pa >> 0);
		fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 4, 0, 0);
		spinand_wait_for_busy(ctx, pdat);
		tx[0] = OPCODE_READ_PAGE_FROM_CACHE;
		tx[1] = (uint8_t)(ca >> 8);
		tx[2] = (uint8_t)(ca >> 0);
		tx[3] = 0x0;
		fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 4, buf, n);
		addr += n;
		buf += n;
		count -= n;
	}
}

static void spinand_helper_erase(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint64_t addr, uint64_t count)
{
	uint64_t base = addr;
	int64_t cnt = count;
	uint32_t esize = pdat->info.page_size * pdat->info.pages_per_block;
	uint32_t emask = esize - 1;
	uint8_t cbuf[256];
	uint32_t clen;
	uint32_t pa;

	cnt += (base & emask);
	base &= (~emask);
	while(cnt > 0)
	{
		pa = base / pdat->info.page_size;
		clen = 0;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = OPCODE_WRITE_ENABLE;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 4;
		cbuf[clen++] = OPCODE_BLOCK_ERASE;
		cbuf[clen++] = (uint8_t)(pa >> 16);
		cbuf[clen++] = (uint8_t)(pa >> 8);
		cbuf[clen++] = (uint8_t)(pa >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		base += esize;
		cnt -= esize;
	}
}

static void spinand_helper_write(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint32_t addr, uint8_t * buf, uint32_t count)
{
	uint8_t * cbuf;
	uint32_t clen;
	uint8_t * txbuf;
	uint32_t txlen;
	uint32_t n;
	uint32_t pa, ca;

	cbuf = malloc(pdat->cmdlen);
	txbuf = malloc(pdat->swaplen);
	if(cbuf && txbuf)
	{
		while(count > 0)
		{
			clen = 0;
			txlen = 0;
			while((clen < (pdat->cmdlen - 30 - 1)) && (txlen < (pdat->swaplen - 4096 - 3)))
			{
				pa = addr / pdat->info.page_size;
				ca = addr & (pdat->info.page_size - 1);
				n = count > pdat->info.page_size ? pdat->info.page_size : count;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_FAST;
				cbuf[clen++] = 1;
				cbuf[clen++] = OPCODE_WRITE_ENABLE;
				cbuf[clen++] = SPI_CMD_DESELECT;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_TXBUF;
				cbuf[clen++] = ((pdat->swapbuf + txlen) >>  0) & 0xff;
				cbuf[clen++] = ((pdat->swapbuf + txlen) >>  8) & 0xff;
				cbuf[clen++] = ((pdat->swapbuf + txlen) >> 16) & 0xff;
				cbuf[clen++] = ((pdat->swapbuf + txlen) >> 24) & 0xff;
				cbuf[clen++] = ((n + 3) >>  0) & 0xff;
				cbuf[clen++] = ((n + 3) >>  8) & 0xff;
				cbuf[clen++] = ((n + 3) >> 16) & 0xff;
				cbuf[clen++] = ((n + 3) >> 24) & 0xff;
				cbuf[clen++] = SPI_CMD_DESELECT;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
				cbuf[clen++] = SPI_CMD_DESELECT;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_FAST;
				cbuf[clen++] = 4;
				cbuf[clen++] = OPCODE_PROGRAM_EXECUTE;
				cbuf[clen++] = (uint8_t)(pa >> 16);
				cbuf[clen++] = (uint8_t)(pa >> 8);
				cbuf[clen++] = (uint8_t)(pa >> 0);
				cbuf[clen++] = SPI_CMD_DESELECT;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
				cbuf[clen++] = SPI_CMD_DESELECT;
				txbuf[txlen++] = OPCODE_PROGRAM_LOAD;
				txbuf[txlen++] = (uint8_t)(ca >> 8);
				txbuf[txlen++] = (uint8_t)(ca >> 0);
				memcpy(&txbuf[txlen], buf, n);
				txlen += n;
				addr += n;
				buf += n;
				count -= n;
			}
			cbuf[clen++] = SPI_CMD_END;
			fel_write(ctx, pdat->swapbuf, txbuf, txlen);
			fel_chip_spi_run(ctx, cbuf, clen);
		}
	}
	if(cbuf)
		free(cbuf);
	if(txbuf)
		free(txbuf);
}

uint64_t spinand_detect(struct xfel_ctx_t * ctx)
{
	struct spinand_pdata_t pdat;

	if(spinand_helper_init(ctx, &pdat))
		return (pdat.info.page_size * pdat.info.pages_per_block * pdat.info.blocks_per_die * pdat.info.ndies);
	return 0;
}

int spinand_read(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinand_pdata_t pdat;
	struct progress_t p;
	uint64_t n;

	if(spinand_helper_init(ctx, &pdat))
	{
		progress_start(&p, len);
		while(len > 0)
		{
			n = len > 65536 ? 65536 : len;
			spinand_helper_read(ctx, &pdat, addr, buf, n);
			addr += n;
			buf += n;
			len -= n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		return 1;
	}
	return 0;
}

int spinand_write(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinand_pdata_t pdat;
	struct progress_t p;
	uint64_t n;

	if(spinand_helper_init(ctx, &pdat))
	{
		progress_start(&p, len);
		spinand_helper_erase(ctx, &pdat, addr, len);
		while(len > 0)
		{
			n = len > 65536 ? 65536 : len;
			spinand_helper_write(ctx, &pdat, addr, buf, n);
			addr += n;
			buf += n;
			len -= n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		return 1;
	}
	return 0;
}
