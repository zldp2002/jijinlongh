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
	OPCODE_FEATURE_PROTECT		= 0xa0,
	OPCODE_FEATURE_CONFIG		= 0xb0,
	OPCODE_FEATURE_STATUS		= 0xc0,
	OPCODE_READ_PAGE_TO_CACHE	= 0x13,
	OPCODE_READ_PAGE_FROM_CACHE	= 0x03,
	OPCODE_WRITE_ENABLE			= 0x06,
	OPCODE_BLOCK_ERASE			= 0xd8,
	OPCODE_PROGRAM_LOAD			= 0x02,
	OPCODE_PROGRAM_EXEC			= 0x10,
	OPCODE_RESET				= 0xff,
};

static const struct spinand_info_t spinand_infos[] = {
	/* Gigadevice */
	{ "GD5F1GQ4UAWxx",   0xc810, 2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExIG",   0xc8d1, 2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExxH",   0xc8d9, 2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ4xAYIG",   0xc8f1, 2048,  64,  64, 1024, 1, 1 },
	{ "GD5F2GQ4UExIG",   0xc8d2, 2048, 128,  64, 2048, 1, 1 },
	{ "GD5F2GQ5UExxH",   0xc832, 2048,  64,  64, 2048, 1, 1 },
	{ "GD5F2GQ4xAYIG",   0xc8f2, 2048,  64,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UBxIG",   0xc8d4, 4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4xAYIG",   0xc8f4, 2048,  64,  64, 4096, 1, 1 },
	{ "GD5F2GQ5UExxG",   0xc852, 2048, 128,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UCxIG",   0xc8b4, 4096, 256,  64, 2048, 1, 1 },

	/* Macronix */
	{ "MX35LF1GE4AB",    0xc212, 2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF1G24AD",    0xc214, 2048, 128,  64, 1024, 1, 1 },
	{ "MX31LF1GE4BC",    0xc21e, 2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF2GE4AB",    0xc222, 2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF2G24AD",    0xc224, 2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2GE4AD",    0xc226, 2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2G14AC",    0xc220, 2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF4G24AD",    0xc235, 4096, 256,  64, 2048, 1, 1 },
	{ "MX35LF4GE4AD",    0xc237, 4096, 256,  64, 2048, 1, 1 },

	/* Micron */
	{ "MT29F1G01AAADD",  0x2c12, 2048,  64,  64, 1024, 1, 1 },
	{ "MT29F1G01ABAFD",  0x2c14, 2048, 128,  64, 1024, 1, 1 },
	{ "MT29F2G01AAAED",  0x2c9f, 2048,  64,  64, 2048, 2, 1 },
	{ "MT29F2G01ABAGD",  0x2c24, 2048, 128,  64, 2048, 2, 1 },
	{ "MT29F4G01AAADD",  0x2c32, 2048,  64,  64, 4096, 2, 1 },
	{ "MT29F4G01ABAFD",  0x2c34, 4096, 256,  64, 2048, 1, 1 },
	{ "MT29F4G01ADAGD",  0x2c36, 2048, 128,  64, 2048, 2, 2 },
	{ "MT29F8G01ADAFD",  0x2c46, 4096, 256,  64, 2048, 1, 2 },

	/* Toshiba */
	{ "TC58CVG0S3HRAIG", 0x98c2, 2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIG", 0x98cb, 2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIG", 0x98cd, 4096, 256,  64, 2048, 1, 1 },
	{ "TC58CVG0S3HRAIJ", 0x98e2, 2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIJ", 0x98eb, 2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIJ", 0x98ed, 4096, 256,  64, 2048, 1, 1 },
	{ "TH58CVG3S0HRAIJ", 0x98e4, 4096, 256,  64, 4096, 1, 1 },

	/* Esmt */
	{ "F50L512M41A",     0xc820, 2048,  64,  64,  512, 1, 1 },
	{ "F50L1G41A",       0xc821, 2048,  64,  64, 1024, 1, 1 },
	{ "F50L1G41LB",      0xc801, 2048,  64,  64, 1024, 1, 1 },
	{ "F50L2G41LB",      0xc80a, 2048,  64,  64, 1024, 1, 2 },

	/* Fison */
	{ "CS11G0T0A0AA",    0x6b00, 2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0G0A0AA",    0x6b10, 2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0S0A0AA",    0x6b20, 2048,  64,  64, 1024, 1, 1 },
	{ "CS11G1T0A0AA",    0x6b01, 2048, 128,  64, 2048, 1, 1 },
	{ "CS11G1S0A0AA",    0x6b21, 2048,  64,  64, 2048, 1, 1 },
	{ "CS11G2T0A0AA",    0x6b02, 2048, 128,  64, 4096, 1, 1 },
	{ "CS11G2S0A0AA",    0x6b22, 2048,  64,  64, 4096, 1, 1 },

	/* Etron */
	{ "EM73B044VCA",     0xd501, 2048,  64,  64,  512, 1, 1 },
	{ "EM73C044SNB",     0xd511, 2048, 120,  64, 1024, 1, 1 },
	{ "EM73C044SNF",     0xd509, 2048, 128,  64, 1024, 1, 1 },
	{ "EM73C044VCA",     0xd518, 2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNA",     0xd519, 2048,  64, 128,  512, 1, 1 },
	{ "EM73C044VCD",     0xd51c, 2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SND",     0xd51d, 2048,  64,  64, 1024, 1, 1 },
	{ "EM73D044SND",     0xd51e, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73C044VCC",     0xd522, 2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044VCF",     0xd525, 2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNC",     0xd531, 2048, 128,  64, 1024, 1, 1 },
	{ "EM73D044SNC",     0xd50a, 2048, 120,  64, 2048, 1, 1 },
	{ "EM73D044SNA",     0xd512, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNF",     0xd510, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCA",     0xd513, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCB",     0xd514, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCD",     0xd517, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCH",     0xd51b, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044SND",     0xd51d, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCG",     0xd51f, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCE",     0xd520, 2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCL",     0xd52e, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNB",     0xd532, 2048, 128,  64, 2048, 1, 1 },
	{ "EM73E044SNA",     0xd503, 4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044SND",     0xd50b, 4096, 240,  64, 2048, 1, 1 },
	{ "EM73E044SNB",     0xd523, 4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCA",     0xd52c, 4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCB",     0xd52f, 2048, 128,  64, 4096, 1, 1 },
	{ "EM73F044SNA",     0xd524, 4096, 256,  64, 4096, 1, 1 },
	{ "EM73F044VCA",     0xd52d, 4096, 256,  64, 4096, 1, 1 },
	{ "EM73E044SNE",     0xd50e, 4096, 256,  64, 4096, 1, 1 },
	{ "EM73C044SNG",     0xd50c, 2048, 120,  64, 1024, 1, 1 },
	{ "EM73D044VCN",     0xd50f, 2048,  64,  64, 2048, 1, 1 },

	/* Elnec */
	{ "FM35Q1GA",        0xe571, 2048,  64,  64, 1024, 1, 1 },

	/* Paragon */
	{ "PN26G01A",        0xa1e1, 2048, 128,  64, 1024, 1, 1 },
	{ "PN26G02A",        0xa1e2, 2048, 128,  64, 2048, 1, 1 },

	/* Ato */
	{ "ATO25D1GA",       0x9b12, 2048,  64,  64, 1024, 1, 1 },

	/* Heyang */
	{ "HYF1GQ4U",        0xc951, 2048, 128,  64, 1024, 1, 1 },
	{ "HYF2GQ4U",        0xc952, 2048, 128,  64, 2048, 1, 1 },
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
	usleep(100 * 1000);
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
		if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, &val) && (val & 0x38))
		{
			spinand_set_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, val & ~0x38);
			spinand_wait_for_busy(ctx, pdat);
		}
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
		spinand_wait_for_busy(ctx, pdat);
		addr += n;
		buf += n;
		count -= n;
	}
}

static void spinand_helper_erase(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat, uint64_t addr, uint64_t count)
{
	uint64_t base;
	int64_t cnt;
	uint32_t esize, emask;
	uint32_t clen;
	uint32_t pa;
	uint8_t cbuf[256];

	esize = pdat->info.page_size * pdat->info.pages_per_block;
	emask = esize - 1;
	base = addr & ~emask;
	cnt = ((addr & emask) + count + esize) & ~emask;
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
		cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
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
	int32_t clen;
	uint8_t * txbuf;
	int32_t txlen;
	int32_t granularity, n;
	uint32_t pa, ca;

	cbuf = malloc(pdat->cmdlen);
	txbuf = malloc(pdat->swaplen);
	if(cbuf && txbuf)
	{
		granularity = pdat->info.page_size > (pdat->swaplen - 3) ? (pdat->swaplen - 3) : pdat->info.page_size;
		while(count > 0)
		{
			clen = 0;
			txlen = 0;
			while((clen < (pdat->cmdlen - 33 - 1)) && (txlen < ((int32_t)pdat->swaplen - granularity - 3)))
			{
				n = count > granularity ? granularity : count;
				pa = addr / pdat->info.page_size;
				ca = addr & (pdat->info.page_size - 1);
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_FAST;
				cbuf[clen++] = 1;
				cbuf[clen++] = OPCODE_WRITE_ENABLE;
				cbuf[clen++] = SPI_CMD_DESELECT;
				cbuf[clen++] = SPI_CMD_SELECT;
				cbuf[clen++] = SPI_CMD_SPINAND_WAIT;
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
				cbuf[clen++] = OPCODE_PROGRAM_EXEC;
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
			if(clen <= pdat->cmdlen)
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
			len -= n;
			buf += n;
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
	uint64_t base, n;
	int64_t cnt;
	uint32_t esize, emask;

	if(spinand_helper_init(ctx, &pdat))
	{
		esize = pdat.info.page_size * pdat.info.pages_per_block;
		emask = esize - 1;
		base = addr & ~emask;
		cnt = ((addr & emask) + len + esize) & ~emask;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > esize ? esize : cnt;
			spinand_helper_erase(ctx, &pdat, base, n);
			base += n;
			cnt -= n;
			progress_update(&p, n);
		}
		base = addr;
		cnt = len;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > 65536 ? 65536 : cnt;
			spinand_helper_write(ctx, &pdat, base, buf, n);
			base += n;
			cnt -= n;
			buf += n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		return 1;
	}
	return 0;
}
