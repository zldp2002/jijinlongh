#include <spinand.h>

#define SPINAND_ID(...)	{ .val = { __VA_ARGS__ }, .len = sizeof((uint8_t[]){ __VA_ARGS__ }) }

struct spinand_info_t {
	char * name;
	struct {
		uint8_t val[4];
		uint8_t len;
	} id;
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
	/* Winbond */
	{ "W25N512GV",       SPINAND_ID(0xef, 0xaa, 0x20), 2048,  64,  64,  512, 1, 1 },
	{ "W25N01GV",        SPINAND_ID(0xef, 0xaa, 0x21), 2048,  64,  64, 1024, 1, 1 },
	{ "W25M02GV",        SPINAND_ID(0xef, 0xab, 0x21), 2048,  64,  64, 1024, 1, 2 },
	{ "W25N02KV",        SPINAND_ID(0xef, 0xaa, 0x22), 2048, 128,  64, 2048, 1, 1 },

	/* Gigadevice */
	{ "GD5F1GQ4UAWxx",   SPINAND_ID(0xc8, 0x10),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ5UExxG",   SPINAND_ID(0xc8, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExIG",   SPINAND_ID(0xc8, 0xd1),       2048, 128,  64, 1024, 1, 1 },
	{ "GD5F1GQ4UExxH",   SPINAND_ID(0xc8, 0xd9),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F1GQ4xAYIG",   SPINAND_ID(0xc8, 0xf1),       2048,  64,  64, 1024, 1, 1 },
	{ "GD5F2GQ4UExIG",   SPINAND_ID(0xc8, 0xd2),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F2GQ5UExxH",   SPINAND_ID(0xc8, 0x32),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F2GQ4xAYIG",   SPINAND_ID(0xc8, 0xf2),       2048,  64,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UBxIG",   SPINAND_ID(0xc8, 0xd4),       4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4xAYIG",   SPINAND_ID(0xc8, 0xf4),       2048,  64,  64, 4096, 1, 1 },
	{ "GD5F2GQ5UExxG",   SPINAND_ID(0xc8, 0x52),       2048, 128,  64, 2048, 1, 1 },
	{ "GD5F4GQ4UCxIG",   SPINAND_ID(0xc8, 0xb4),       4096, 256,  64, 2048, 1, 1 },
	{ "GD5F4GQ4RCxIG",   SPINAND_ID(0xc8, 0xa4),       4096, 256,  64, 2048, 1, 1 },

	/* Macronix */
	{ "MX35LF1GE4AB",    SPINAND_ID(0xc2, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF1G24AD",    SPINAND_ID(0xc2, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MX31LF1GE4BC",    SPINAND_ID(0xc2, 0x1e),       2048,  64,  64, 1024, 1, 1 },
	{ "MX35LF2GE4AB",    SPINAND_ID(0xc2, 0x22),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF2G24AD",    SPINAND_ID(0xc2, 0x24),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2GE4AD",    SPINAND_ID(0xc2, 0x26),       2048, 128,  64, 2048, 1, 1 },
	{ "MX35LF2G14AC",    SPINAND_ID(0xc2, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "MX35LF4G24AD",    SPINAND_ID(0xc2, 0x35),       4096, 256,  64, 2048, 1, 1 },
	{ "MX35LF4GE4AD",    SPINAND_ID(0xc2, 0x37),       4096, 256,  64, 2048, 1, 1 },

	/* Micron */
	{ "MT29F1G01AAADD",  SPINAND_ID(0x2c, 0x12),       2048,  64,  64, 1024, 1, 1 },
	{ "MT29F1G01ABAFD",  SPINAND_ID(0x2c, 0x14),       2048, 128,  64, 1024, 1, 1 },
	{ "MT29F2G01AAAED",  SPINAND_ID(0x2c, 0x9f),       2048,  64,  64, 2048, 2, 1 },
	{ "MT29F2G01ABAGD",  SPINAND_ID(0x2c, 0x24),       2048, 128,  64, 2048, 2, 1 },
	{ "MT29F4G01AAADD",  SPINAND_ID(0x2c, 0x32),       2048,  64,  64, 4096, 2, 1 },
	{ "MT29F4G01ABAFD",  SPINAND_ID(0x2c, 0x34),       4096, 256,  64, 2048, 1, 1 },
	{ "MT29F4G01ADAGD",  SPINAND_ID(0x2c, 0x36),       2048, 128,  64, 2048, 2, 2 },
	{ "MT29F8G01ADAFD",  SPINAND_ID(0x2c, 0x46),       4096, 256,  64, 2048, 1, 2 },

	/* Toshiba */
	{ "TC58CVG0S3HRAIG", SPINAND_ID(0x98, 0xc2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIG", SPINAND_ID(0x98, 0xcb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIG", SPINAND_ID(0x98, 0xcd),       4096, 256,  64, 2048, 1, 1 },
	{ "TC58CVG0S3HRAIJ", SPINAND_ID(0x98, 0xe2),       2048, 128,  64, 1024, 1, 1 },
	{ "TC58CVG1S3HRAIJ", SPINAND_ID(0x98, 0xeb),       2048, 128,  64, 2048, 1, 1 },
	{ "TC58CVG2S0HRAIJ", SPINAND_ID(0x98, 0xed),       4096, 256,  64, 2048, 1, 1 },
	{ "TH58CVG3S0HRAIJ", SPINAND_ID(0x98, 0xe4),       4096, 256,  64, 4096, 1, 1 },

	/* Esmt */
	{ "F50L512M41A",     SPINAND_ID(0xc8, 0x20),       2048,  64,  64,  512, 1, 1 },
	{ "F50L1G41A",       SPINAND_ID(0xc8, 0x21),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L1G41LB",      SPINAND_ID(0xc8, 0x01),       2048,  64,  64, 1024, 1, 1 },
	{ "F50L2G41LB",      SPINAND_ID(0xc8, 0x0a),       2048,  64,  64, 1024, 1, 2 },

	/* Fison */
	{ "CS11G0T0A0AA",    SPINAND_ID(0x6b, 0x00),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0G0A0AA",    SPINAND_ID(0x6b, 0x10),       2048, 128,  64, 1024, 1, 1 },
	{ "CS11G0S0A0AA",    SPINAND_ID(0x6b, 0x20),       2048,  64,  64, 1024, 1, 1 },
	{ "CS11G1T0A0AA",    SPINAND_ID(0x6b, 0x01),       2048, 128,  64, 2048, 1, 1 },
	{ "CS11G1S0A0AA",    SPINAND_ID(0x6b, 0x21),       2048,  64,  64, 2048, 1, 1 },
	{ "CS11G2T0A0AA",    SPINAND_ID(0x6b, 0x02),       2048, 128,  64, 4096, 1, 1 },
	{ "CS11G2S0A0AA",    SPINAND_ID(0x6b, 0x22),       2048,  64,  64, 4096, 1, 1 },

	/* Etron */
	{ "EM73B044VCA",     SPINAND_ID(0xd5, 0x01),       2048,  64,  64,  512, 1, 1 },
	{ "EM73C044SNB",     SPINAND_ID(0xd5, 0x11),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73C044SNF",     SPINAND_ID(0xd5, 0x09),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73C044VCA",     SPINAND_ID(0xd5, 0x18),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNA",     SPINAND_ID(0xd5, 0x19),       2048,  64, 128,  512, 1, 1 },
	{ "EM73C044VCD",     SPINAND_ID(0xd5, 0x1c),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1e),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73C044VCC",     SPINAND_ID(0xd5, 0x22),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044VCF",     SPINAND_ID(0xd5, 0x25),       2048,  64,  64, 1024, 1, 1 },
	{ "EM73C044SNC",     SPINAND_ID(0xd5, 0x31),       2048, 128,  64, 1024, 1, 1 },
	{ "EM73D044SNC",     SPINAND_ID(0xd5, 0x0a),       2048, 120,  64, 2048, 1, 1 },
	{ "EM73D044SNA",     SPINAND_ID(0xd5, 0x12),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNF",     SPINAND_ID(0xd5, 0x10),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCA",     SPINAND_ID(0xd5, 0x13),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCB",     SPINAND_ID(0xd5, 0x14),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCD",     SPINAND_ID(0xd5, 0x17),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044VCH",     SPINAND_ID(0xd5, 0x1b),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044SND",     SPINAND_ID(0xd5, 0x1d),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCG",     SPINAND_ID(0xd5, 0x1f),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCE",     SPINAND_ID(0xd5, 0x20),       2048,  64,  64, 2048, 1, 1 },
	{ "EM73D044VCL",     SPINAND_ID(0xd5, 0x2e),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73D044SNB",     SPINAND_ID(0xd5, 0x32),       2048, 128,  64, 2048, 1, 1 },
	{ "EM73E044SNA",     SPINAND_ID(0xd5, 0x03),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044SND",     SPINAND_ID(0xd5, 0x0b),       4096, 240,  64, 2048, 1, 1 },
	{ "EM73E044SNB",     SPINAND_ID(0xd5, 0x23),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCA",     SPINAND_ID(0xd5, 0x2c),       4096, 256,  64, 2048, 1, 1 },
	{ "EM73E044VCB",     SPINAND_ID(0xd5, 0x2f),       2048, 128,  64, 4096, 1, 1 },
	{ "EM73F044SNA",     SPINAND_ID(0xd5, 0x24),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73F044VCA",     SPINAND_ID(0xd5, 0x2d),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73E044SNE",     SPINAND_ID(0xd5, 0x0e),       4096, 256,  64, 4096, 1, 1 },
	{ "EM73C044SNG",     SPINAND_ID(0xd5, 0x0c),       2048, 120,  64, 1024, 1, 1 },
	{ "EM73D044VCN",     SPINAND_ID(0xd5, 0x0f),       2048,  64,  64, 2048, 1, 1 },

	/* Elnec */
	{ "FM35Q1GA",        SPINAND_ID(0xe5, 0x71),       2048,  64,  64, 1024, 1, 1 },

	/* Paragon */
	{ "PN26G01A",        SPINAND_ID(0xa1, 0xe1),       2048, 128,  64, 1024, 1, 1 },
	{ "PN26G02A",        SPINAND_ID(0xa1, 0xe2),       2048, 128,  64, 2048, 1, 1 },

	/* Ato */
	{ "ATO25D1GA",       SPINAND_ID(0x9b, 0x12),       2048,  64,  64, 1024, 1, 1 },

	/* Heyang */
	{ "HYF1GQ4U",        SPINAND_ID(0xc9, 0x51),       2048, 128,  64, 1024, 1, 1 },
	{ "HYF2GQ4U",        SPINAND_ID(0xc9, 0x52),       2048, 128,  64, 2048, 1, 1 },
};

static inline int spinand_info(struct xfel_ctx_t * ctx, struct spinand_pdata_t * pdat)
{
	const struct spinand_info_t * t;
	uint8_t tx[2];
	uint8_t rx[4];
	int i;

	tx[0] = OPCODE_RDID;
	tx[1] = 0x0;
	if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 2, rx, 4))
		return 0;
	for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
	{
		t = &spinand_infos[i];
		if(memcmp(rx, t->id.val, t->id.len) == 0)
		{
			memcpy(&pdat->info, t, sizeof(struct spinand_info_t));
			return 1;
		}
	}
	tx[0] = OPCODE_RDID;
	if(!fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 1, rx, 4))
		return 0;
	for(i = 0; i < ARRAY_SIZE(spinand_infos); i++)
	{
		t = &spinand_infos[i];
		if(memcmp(rx, t->id.val, t->id.len) == 0)
		{
			memcpy(&pdat->info, t, sizeof(struct spinand_info_t));
			return 1;
		}
	}
	printf("The spi nand flash '0x%02x%02x%02x%02x' is not yet supported\r\n", rx[0], rx[1], rx[2], rx[3]);
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
		if(spinand_get_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, &val) && (val != 0x0))
		{
			spinand_set_feature(ctx, pdat, OPCODE_FEATURE_PROTECT, 0x0);
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
	cnt = (addr & emask) + count;
	cnt = (cnt + ((cnt & emask) ? esize : 0)) & ~emask;
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

int spinand_detect(struct xfel_ctx_t * ctx, char * name, uint64_t * capacity)
{
	struct spinand_pdata_t pdat;

	if(spinand_helper_init(ctx, &pdat))
	{
		if(name)
			strcpy(name, pdat.info.name);
		if(capacity)
			*capacity = pdat.info.page_size * pdat.info.pages_per_block * pdat.info.blocks_per_die * pdat.info.ndies;
		return 1;
	}
	return 0;
}

int spinand_erase(struct xfel_ctx_t * ctx, uint64_t addr, uint64_t len)
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
		cnt = (addr & emask) + len;
		cnt = (cnt + ((cnt & emask) ? esize : 0)) & ~emask;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > esize ? esize : cnt;
			spinand_helper_erase(ctx, &pdat, base, n);
			base += n;
			cnt -= n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		return 1;
	}
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
		cnt = (addr & emask) + len;
		cnt = (cnt + ((cnt & emask) ? esize : 0)) & ~emask;
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

int spinand_splwrite(struct xfel_ctx_t * ctx, uint32_t splitsz, uint64_t addr, void * buf, uint64_t len)
{
	struct spinand_pdata_t pdat;
	struct progress_t p;
	uint64_t base, n;
	int64_t cnt;
	uint32_t esize, emask;
	void * nbuf;
	uint64_t nlen;

	if(spinand_helper_init(ctx, &pdat))
	{
		esize = pdat.info.page_size * pdat.info.pages_per_block;
		emask = esize - 1;
		if((splitsz <= 0) || (splitsz > pdat.info.page_size))
			splitsz = pdat.info.page_size;
		if(splitsz & 0x3ff)
		{
			printf("The valid page size is not 1k alignable and thus not supported\r\n");
			return 0;
		}
		uint8_t * pbuf = buf;
		if(memcmp(&pbuf[4], "eGON.BT0", 8) != 0)
		{
			printf("Invalid a eGON boot image\r\n");
			return 0;
		}
		uint32_t splsz = (pbuf[19] << 24) | (pbuf[18] << 16) | (pbuf[17] << 8) | (pbuf[16] << 0);
		if(splsz > len)
		{
			printf("The spl size is too large, please check!\r\n");
			return 0;
		}
		uint32_t tsplsz = (splsz * pdat.info.page_size / splitsz + esize) & ~emask;
		if(addr >= tsplsz)
		{
			int copies = 0;
			nlen = 0;
			while(nlen < addr)
			{
				nlen += tsplsz;
				copies++;
			}
			nlen += len;
			nbuf = malloc(nlen);
			if(nbuf)
			{
				uint8_t * pb = buf;
				uint8_t * pnb = nbuf;
				memset(pnb, 0xff, nlen);
				for(int i = 0; i < splsz; i += splitsz)
				{
					memcpy(pnb, pb, splitsz);
					pb += splitsz;
					pnb += pdat.info.page_size;
				}
				for(int i = 1; i < copies; i++)
				{
					memcpy(nbuf + tsplsz * i, nbuf, tsplsz);
				}
				memcpy(nbuf + (nlen - len), buf, len);
			}
			else
				return 0;
		}
		else
		{
			nlen = tsplsz;
			nbuf = malloc(nlen);
			if(nbuf)
			{
				uint8_t * pb = buf;
				uint8_t * pnb = nbuf;
				memset(pnb, 0xff, nlen);
				for(int i = 0; i < splsz; i += splitsz)
				{
					memcpy(pnb, pb, splitsz);
					pb += splitsz;
					pnb += pdat.info.page_size;
				}
			}
			else
				return 0;
		}
		uint8_t * pnbuf = nbuf;
		base = 0 & ~emask;
		cnt = ((0 + nlen + esize - 1) & ~emask) - base;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > esize ? esize : cnt;
			spinand_helper_erase(ctx, &pdat, base, n);
			base += n;
			cnt -= n;
			progress_update(&p, n);
		}
		base = 0;
		cnt = nlen;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > 65536 ? 65536 : cnt;
			spinand_helper_write(ctx, &pdat, base, pnbuf, n);
			base += n;
			cnt -= n;
			pnbuf += n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		if(nbuf)
			free(nbuf);
		return 1;
	}
	return 0;
}
