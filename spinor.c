#include <spinor.h>

struct spinor_info_t {
	char * name;
	uint32_t id;
	uint32_t capacity;
	uint32_t blksz;
	uint32_t read_granularity;
	uint32_t write_granularity;
	uint8_t address_length;
	uint8_t opcode_read;
	uint8_t opcode_write;
	uint8_t opcode_write_enable;
	uint8_t opcode_erase_4k;
	uint8_t opcode_erase_32k;
	uint8_t opcode_erase_64k;
	uint8_t opcode_erase_256k;
};

struct spinor_pdata_t {
	struct spinor_info_t info;
	uint32_t swapbuf;
	uint32_t swaplen;
	uint32_t cmdlen;
};

enum {
	OPCODE_SFDP			= 0x5a,
	OPCODE_RDID			= 0x9f,
	OPCODE_WRSR			= 0x01,
	OPCODE_RDSR			= 0x05,
	OPCODE_WREN			= 0x06,
	OPCODE_READ			= 0x03,
	OPCODE_PROG			= 0x02,
	OPCODE_E4K			= 0x20,
	OPCODE_E32K			= 0x52,
	OPCODE_E64K			= 0xd8,
	OPCODE_ENTER_4B		= 0xb7,
	OPCODE_EXIT_4B		= 0xe9,
};
#define SFDP_MAX_NPH	(6)

struct sfdp_header_t {
	uint8_t sign[4];
	uint8_t minor;
	uint8_t major;
	uint8_t nph;
	uint8_t unused;
};

struct sfdp_parameter_header_t {
	uint8_t idlsb;
	uint8_t minor;
	uint8_t major;
	uint8_t length;
	uint8_t ptp[3];
	uint8_t idmsb;
};

struct sfdp_basic_table_t {
	uint8_t minor;
	uint8_t major;
	uint8_t table[16 * 4];
};

struct sfdp_t {
	struct sfdp_header_t h;
	struct sfdp_parameter_header_t ph[SFDP_MAX_NPH];
	struct sfdp_basic_table_t bt;
};

static const struct spinor_info_t spinor_infos[] = {
	{ "W25X40", 0xef3013, 512 * 1024, 4096, 1, 256, 3, OPCODE_READ, OPCODE_PROG, OPCODE_WREN, OPCODE_E4K, 0, OPCODE_E64K, 0 },
};

static inline int spinor_read_sfdp(struct xfel_ctx_t * ctx, uint32_t swapbuf, uint32_t swaplen, uint32_t cmdlen, struct sfdp_t * sfdp)
{
	uint32_t addr;
	uint8_t tx[5];
	int i;

	memset(sfdp, 0, sizeof(struct sfdp_t));
	tx[0] = OPCODE_SFDP;
	tx[1] = 0x0;
	tx[2] = 0x0;
	tx[3] = 0x0;
	tx[4] = 0x0;
	if(!fel_spi_xfer(ctx, swapbuf, swaplen, cmdlen, tx, 5, &sfdp->h, sizeof(struct sfdp_header_t)))
		return 0;
	if((sfdp->h.sign[0] != 'S') || (sfdp->h.sign[1] != 'F') || (sfdp->h.sign[2] != 'D') || (sfdp->h.sign[3] != 'P'))
		return 0;
	sfdp->h.nph = sfdp->h.nph > SFDP_MAX_NPH ? sfdp->h.nph + 1 : SFDP_MAX_NPH;
	for(i = 0; i < sfdp->h.nph; i++)
	{
		addr = i * sizeof(struct sfdp_parameter_header_t) + sizeof(struct sfdp_header_t);
		tx[0] = OPCODE_SFDP;
		tx[1] = (addr >> 16) & 0xff;
		tx[2] = (addr >>  8) & 0xff;
		tx[3] = (addr >>  0) & 0xff;
		tx[4] = 0x0;
		if(!fel_spi_xfer(ctx, swapbuf, swaplen, cmdlen, tx, 5, &sfdp->ph[i], sizeof(struct sfdp_parameter_header_t)))
			return 0;
	}
	for(i = 0; i < sfdp->h.nph; i++)
	{
		if((sfdp->ph[i].idlsb == 0x00) && (sfdp->ph[i].idmsb == 0xff))
		{
			addr = (sfdp->ph[i].ptp[0] << 0) | (sfdp->ph[i].ptp[1] << 8) | (sfdp->ph[i].ptp[2] << 16);
			tx[0] = OPCODE_SFDP;
			tx[1] = (addr >> 16) & 0xff;
			tx[2] = (addr >>  8) & 0xff;
			tx[3] = (addr >>  0) & 0xff;
			tx[4] = 0x0;
			if(fel_spi_xfer(ctx, swapbuf, swaplen, cmdlen, tx, 5, &sfdp->bt.table[0], sfdp->ph[i].length * 4))
			{
				sfdp->bt.major = sfdp->ph[i].major;
				sfdp->bt.minor = sfdp->ph[i].minor;
				return 1;
			}
		}
	}
	return 0;
}

static inline int spinor_read_id(struct xfel_ctx_t * ctx, uint32_t swapbuf, uint32_t swaplen, uint32_t cmdlen, uint32_t * id)
{
	uint8_t tx[1];
	uint8_t rx[3];

	tx[0] = OPCODE_RDID;
	if(!fel_spi_xfer(ctx, swapbuf, swaplen, cmdlen, tx, 1, rx, 3))
		return 0;
	*id = (rx[0] << 16) | (rx[1] << 8) | (rx[2] << 0);
	return 1;
}

static inline int spinor_info(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat)
{
	const struct spinor_info_t * t;
	struct sfdp_t sfdp;
	uint32_t v, id;
	int i;

	if(spinor_read_sfdp(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, &sfdp))
	{
		pdat->info.name = "SFDP";
		pdat->info.id = 0;
		/* Basic flash parameter table 2th dword */
		v = (sfdp.bt.table[7] << 24) | (sfdp.bt.table[6] << 16) | (sfdp.bt.table[5] << 8) | (sfdp.bt.table[4] << 0);
		if(v & (1 << 31))
		{
			v &= 0x7fffffff;
			pdat->info.capacity = 1 << (v - 3);
		}
		else
		{
			pdat->info.capacity = (v + 1) >> 3;
		}
		/* Basic flash parameter table 1th dword */
		v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
		if((pdat->info.capacity <= (16 * 1024 * 1024)) && (((v >> 17) & 0x3) != 0x2))
			pdat->info.address_length = 3;
		else
			pdat->info.address_length = 4;
		if(((v >> 0) & 0x3) == 0x1)
			pdat->info.opcode_erase_4k = (v >> 8) & 0xff;
		else
			pdat->info.opcode_erase_4k = 0x00;
		pdat->info.opcode_erase_32k = 0x00;
		pdat->info.opcode_erase_64k = 0x00;
		pdat->info.opcode_erase_256k = 0x00;
		/* Basic flash parameter table 8th dword */
		v = (sfdp.bt.table[31] << 24) | (sfdp.bt.table[30] << 16) | (sfdp.bt.table[29] << 8) | (sfdp.bt.table[28] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			pdat->info.opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			pdat->info.opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			pdat->info.opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			pdat->info.opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			pdat->info.opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			pdat->info.opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			pdat->info.opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			pdat->info.opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		/* Basic flash parameter table 9th dword */
		v = (sfdp.bt.table[35] << 24) | (sfdp.bt.table[34] << 16) | (sfdp.bt.table[33] << 8) | (sfdp.bt.table[32] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			pdat->info.opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			pdat->info.opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			pdat->info.opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			pdat->info.opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			pdat->info.opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			pdat->info.opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			pdat->info.opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			pdat->info.opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		if(pdat->info.opcode_erase_4k != 0x00)
			pdat->info.blksz = 4096;
		else if(pdat->info.opcode_erase_32k != 0x00)
			pdat->info.blksz = 32768;
		else if(pdat->info.opcode_erase_64k != 0x00)
			pdat->info.blksz = 65536;
		else if(pdat->info.opcode_erase_256k != 0x00)
			pdat->info.blksz = 262144;
		pdat->info.opcode_write_enable = OPCODE_WREN;
		pdat->info.read_granularity = 1;
		pdat->info.opcode_read = OPCODE_READ;
		if((sfdp.bt.major == 1) && (sfdp.bt.minor < 5))
		{
			/* Basic flash parameter table 1th dword */
			v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
			if((v >> 2) & 0x1)
				pdat->info.write_granularity = 64;
			else
				pdat->info.write_granularity = 1;
		}
		else if((sfdp.bt.major == 1) && (sfdp.bt.minor >= 5))
		{
			/* Basic flash parameter table 11th dword */
			v = (sfdp.bt.table[43] << 24) | (sfdp.bt.table[42] << 16) | (sfdp.bt.table[41] << 8) | (sfdp.bt.table[40] << 0);
			pdat->info.write_granularity = 1 << ((v >> 4) & 0xf);
		}
		pdat->info.opcode_write = OPCODE_PROG;
		return 1;
	}
	else if(spinor_read_id(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, &id) && (id != 0xffffff) && (id != 0))
	{
		for(i = 0; i < ARRAY_SIZE(spinor_infos); i++)
		{
			t = &spinor_infos[i];
			if(id == t->id)
			{
				memcpy(&pdat->info, t, sizeof(struct spinor_info_t));
				return 1;
			}
		}
		printf("The spi nor flash '0x%x' is not yet supported\r\n", id);
	}
	return 0;
}

static int spinor_helper_init(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	if(fel_spi_init(ctx, &pdat->swapbuf, &pdat->swaplen, &pdat->cmdlen) && spinor_info(ctx, pdat))
	{
		/* spi select */
		cbuf[clen++] = SPI_CMD_SELECT;
		/* chip reset */
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 2;
		cbuf[clen++] = 0x66;
		cbuf[clen++] = 0x99;
		/* spi deselect */
		cbuf[clen++] = SPI_CMD_DESELECT;

		/* spi select */
		cbuf[clen++] = SPI_CMD_SELECT;
		/* wait busy */
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		/* spi deselect */
		cbuf[clen++] = SPI_CMD_DESELECT;

		/* spi select */
		cbuf[clen++] = SPI_CMD_SELECT;
		/* write enable */
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		/* spi deselect */
		cbuf[clen++] = SPI_CMD_DESELECT;

		/* spi select */
		cbuf[clen++] = SPI_CMD_SELECT;
		/* write status */
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 2;
		cbuf[clen++] = OPCODE_WRSR;
		cbuf[clen++] = 0;
		/* spi deselect */
		cbuf[clen++] = SPI_CMD_DESELECT;

		/* spi select */
		cbuf[clen++] = SPI_CMD_SELECT;
		/* wait busy */
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		/* spi deselect */
		cbuf[clen++] = SPI_CMD_DESELECT;

		if(pdat->info.address_length == 4)
		{
			/* spi select */
			cbuf[clen++] = SPI_CMD_SELECT;
			/* write enable */
			cbuf[clen++] = SPI_CMD_FAST;
			cbuf[clen++] = 1;
			cbuf[clen++] = pdat->info.opcode_write_enable;
			/* spi deselect */
			cbuf[clen++] = SPI_CMD_DESELECT;

			/* spi select */
			cbuf[clen++] = SPI_CMD_SELECT;
			/* entern 4byte address */
			cbuf[clen++] = SPI_CMD_FAST;
			cbuf[clen++] = 1;
			cbuf[clen++] = OPCODE_ENTER_4B;
			/* spi deselect */
			cbuf[clen++] = SPI_CMD_DESELECT;

			/* spi select */
			cbuf[clen++] = SPI_CMD_SELECT;
			/* wait busy */
			cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
			/* spi deselect */
			cbuf[clen++] = SPI_CMD_DESELECT;
		}

		/* end */
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
		{
			fel_chip_spi_run(ctx, cbuf, clen);
			return 1;
		}
	}
	return 0;
}

static void spinor_helper_read(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr, uint8_t * buf, uint32_t count)
{
	int32_t granularity, n;
	uint8_t tx[5];

	if(pdat->info.read_granularity == 1)
		granularity = (count < 0x40000000) ? count : 0x40000000;
	else
		granularity = pdat->info.read_granularity;
	switch(pdat->info.address_length)
	{
	case 3:
		while(count > 0)
		{
			n = count > granularity ? granularity : count;
			tx[0] = pdat->info.opcode_read;
			tx[1] = (uint8_t)(addr >> 16);
			tx[2] = (uint8_t)(addr >> 8);
			tx[3] = (uint8_t)(addr >> 0);
			fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 4, buf, n);
			addr += n;
			buf += n;
			count -= n;
		}
		break;
	case 4:
		while(count > 0)
		{
			n = count > granularity ? granularity : count;
			tx[0] = pdat->info.opcode_read;
			tx[1] = (uint8_t)(addr >> 24);
			tx[2] = (uint8_t)(addr >> 16);
			tx[3] = (uint8_t)(addr >> 8);
			tx[4] = (uint8_t)(addr >> 0);
			fel_spi_xfer(ctx, pdat->swapbuf, pdat->swaplen, pdat->cmdlen, tx, 5, buf, n);
			addr += n;
			buf += n;
			count -= n;
		}
		break;
	default:
		break;
	}
}

static inline void spinor_sector_erase_4k(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	switch(pdat->info.address_length)
	{
	case 3:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 4;
		cbuf[clen++] = pdat->info.opcode_erase_4k;
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	case 4:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 5;
		cbuf[clen++] = pdat->info.opcode_erase_4k;
		cbuf[clen++] = (uint8_t)(addr >> 24);
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	default:
		break;
	}
}

static inline void spinor_sector_erase_32k(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	switch(pdat->info.address_length)
	{
	case 3:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 4;
		cbuf[clen++] = pdat->info.opcode_erase_32k;
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	case 4:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 5;
		cbuf[clen++] = pdat->info.opcode_erase_32k;
		cbuf[clen++] = (uint8_t)(addr >> 24);
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	default:
		break;
	}
}

static inline void spinor_sector_erase_64k(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	switch(pdat->info.address_length)
	{
	case 3:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 4;
		cbuf[clen++] = pdat->info.opcode_erase_64k;
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	case 4:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 5;
		cbuf[clen++] = pdat->info.opcode_erase_64k;
		cbuf[clen++] = (uint8_t)(addr >> 24);
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	default:
		break;
	}
}

static inline void spinor_sector_erase_256k(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr)
{
	uint8_t cbuf[256];
	uint32_t clen = 0;

	switch(pdat->info.address_length)
	{
	case 3:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 4;
		cbuf[clen++] = pdat->info.opcode_erase_256k;
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	case 4:
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 1;
		cbuf[clen++] = pdat->info.opcode_write_enable;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_FAST;
		cbuf[clen++] = 5;
		cbuf[clen++] = pdat->info.opcode_erase_256k;
		cbuf[clen++] = (uint8_t)(addr >> 24);
		cbuf[clen++] = (uint8_t)(addr >> 16);
		cbuf[clen++] = (uint8_t)(addr >> 8);
		cbuf[clen++] = (uint8_t)(addr >> 0);
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(clen <= pdat->cmdlen)
			fel_chip_spi_run(ctx, cbuf, clen);
		break;
	default:
		break;
	}
}

static void spinor_helper_erase(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint64_t addr, uint64_t count)
{
	uint64_t base;
	int64_t cnt;
	uint32_t esize, emask;
	uint32_t len;

	if(pdat->info.opcode_erase_4k != 0)
		esize = 4096;
	else if(pdat->info.opcode_erase_32k != 0)
		esize = 32768;
	else if(pdat->info.opcode_erase_32k != 0)
		esize = 65536;
	else if(pdat->info.opcode_erase_32k != 0)
		esize = 262144;
	else
		return;
	emask = esize - 1;

	base = addr & ~emask;
	cnt = ((addr & emask) + count + esize) & ~emask;
	while(cnt > 0)
	{
		if((pdat->info.opcode_erase_256k != 0) && ((base & 0x3ffff) == 0) && (cnt >= 262144))
		{
			len = 262144;
			spinor_sector_erase_256k(ctx, pdat, base);
		}
		else if((pdat->info.opcode_erase_64k != 0) && ((base & 0xffff) == 0) && (cnt >= 65536))
		{
			len = 65536;
			spinor_sector_erase_64k(ctx, pdat, base);
		}
		else if((pdat->info.opcode_erase_32k != 0) && ((base & 0x7fff) == 0) && (cnt >= 32768))
		{
			len = 32768;
			spinor_sector_erase_32k(ctx, pdat, base);
		}
		else if((pdat->info.opcode_erase_4k != 0) && ((base & 0xfff) == 0) && (cnt >= 4096))
		{
			len = 4096;
			spinor_sector_erase_4k(ctx, pdat, base);
		}
		else
			return;
		base += len;
		cnt -= len;
	}
}

static void spinor_helper_write(struct xfel_ctx_t * ctx, struct spinor_pdata_t * pdat, uint32_t addr, uint8_t * buf, uint32_t count)
{
	uint8_t * cbuf;
	int32_t clen;
	uint8_t * txbuf;
	int32_t txlen;
	int32_t granularity, n;

	if(pdat->info.write_granularity == 1)
		granularity = (count < 0x40000000) ? count : 0x40000000;
	else
		granularity = pdat->info.write_granularity;
	granularity = granularity > (pdat->swaplen - 5) ? (pdat->swaplen - 5) : granularity;

	switch(pdat->info.address_length)
	{
	case 3:
		cbuf = malloc(pdat->cmdlen);
		txbuf = malloc(pdat->swaplen);
		if(cbuf && txbuf)
		{
			while(count > 0)
			{
				clen = 0;
				txlen = 0;
				while((clen < (pdat->cmdlen - 19 - 1)) && (txlen < ((int32_t)pdat->swaplen - granularity - 4)))
				{
					n = count > granularity ? granularity : count;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_FAST;
					cbuf[clen++] = 1;
					cbuf[clen++] = pdat->info.opcode_write_enable;
					cbuf[clen++] = SPI_CMD_DESELECT;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_TXBUF;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >>  0) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >>  8) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >> 16) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >> 24) & 0xff;
					cbuf[clen++] = ((n + 4) >>  0) & 0xff;
					cbuf[clen++] = ((n + 4) >>  8) & 0xff;
					cbuf[clen++] = ((n + 4) >> 16) & 0xff;
					cbuf[clen++] = ((n + 4) >> 24) & 0xff;
					cbuf[clen++] = SPI_CMD_DESELECT;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
					cbuf[clen++] = SPI_CMD_DESELECT;
					txbuf[txlen++] = pdat->info.opcode_write;
					txbuf[txlen++] = (uint8_t)(addr >> 16);
					txbuf[txlen++] = (uint8_t)(addr >> 8);
					txbuf[txlen++] = (uint8_t)(addr >> 0);
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
		break;
	case 4:
		cbuf = malloc(pdat->cmdlen);
		txbuf = malloc(pdat->swaplen);
		if(cbuf && txbuf)
		{
			while(count > 0)
			{
				clen = 0;
				txlen = 0;
				while((clen < (pdat->cmdlen - 19 - 1)) && (txlen < ((int32_t)pdat->swaplen - granularity - 5)))
				{
					n = count > granularity ? granularity : count;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_FAST;
					cbuf[clen++] = 1;
					cbuf[clen++] = pdat->info.opcode_write_enable;
					cbuf[clen++] = SPI_CMD_DESELECT;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_TXBUF;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >>  0) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >>  8) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >> 16) & 0xff;
					cbuf[clen++] = ((pdat->swapbuf + txlen) >> 24) & 0xff;
					cbuf[clen++] = ((n + 5) >>  0) & 0xff;
					cbuf[clen++] = ((n + 5) >>  8) & 0xff;
					cbuf[clen++] = ((n + 5) >> 16) & 0xff;
					cbuf[clen++] = ((n + 5) >> 24) & 0xff;
					cbuf[clen++] = SPI_CMD_DESELECT;
					cbuf[clen++] = SPI_CMD_SELECT;
					cbuf[clen++] = SPI_CMD_SPINOR_WAIT;
					cbuf[clen++] = SPI_CMD_DESELECT;
					txbuf[txlen++] = pdat->info.opcode_write;
					txbuf[txlen++] = (uint8_t)(addr >> 24);
					txbuf[txlen++] = (uint8_t)(addr >> 16);
					txbuf[txlen++] = (uint8_t)(addr >> 8);
					txbuf[txlen++] = (uint8_t)(addr >> 0);
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
		break;
	default:
		break;
	}
}

int spinor_detect(struct xfel_ctx_t * ctx, char * name, uint64_t * capacity)
{
	struct spinor_pdata_t pdat;

	if(spinor_helper_init(ctx, &pdat))
	{
		if(name)
			strcpy(name, pdat.info.name);
		if(capacity)
			*capacity = pdat.info.capacity;
		return 1;
	}
	return 0;
}

int spinor_erase(struct xfel_ctx_t * ctx, uint64_t addr, uint64_t len)
{
	struct spinor_pdata_t pdat;
	struct progress_t p;
	uint64_t base, n;
	int64_t cnt;
	uint32_t esize, emask;

	if(spinor_helper_init(ctx, &pdat))
	{
		if(pdat.info.opcode_erase_4k != 0)
			esize = 4096;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 32768;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 65536;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 262144;
		else
			return 0;
		emask = esize - 1;
		base = addr & ~emask;
		cnt = ((addr & emask) + len + ((addr == base) && (len >= esize)) ? 0 : esize) & ~emask;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > 262144 ? 262144 : cnt;
			spinor_helper_erase(ctx, &pdat, base, n);
			base += n;
			cnt -= n;
			progress_update(&p, n);
		}
		progress_stop(&p);
		return 1;
	}
	return 0;
}

int spinor_read(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinor_pdata_t pdat;
	struct progress_t p;
	uint64_t n;

	if(spinor_helper_init(ctx, &pdat))
	{
		progress_start(&p, len);
		while(len > 0)
		{
			n = len > 65536 ? 65536 : len;
			spinor_helper_read(ctx, &pdat, addr, buf, n);
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

int spinor_write(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinor_pdata_t pdat;
	struct progress_t p;
	uint64_t base, n;
	int64_t cnt;
	uint32_t esize, emask;

	if(spinor_helper_init(ctx, &pdat))
	{
		if(pdat.info.opcode_erase_4k != 0)
			esize = 4096;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 32768;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 65536;
		else if(pdat.info.opcode_erase_32k != 0)
			esize = 262144;
		else
			return 0;
		emask = esize - 1;
		base = addr & ~emask;
		cnt = ((addr & emask) + len + ((addr == base) && (len >= esize)) ? 0 : esize) & ~emask;
		progress_start(&p, cnt);
		while(cnt > 0)
		{
			n = cnt > 262144 ? 262144 : cnt;
			spinor_helper_erase(ctx, &pdat, base, n);
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
			spinor_helper_write(ctx, &pdat, base, buf, n);
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
