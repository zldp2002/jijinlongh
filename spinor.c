#include <spinor.h>

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

static const struct spinor_info_t spinor_infos[] = {
	{ "w25x40", 0xef3013, 512 * 1024, 4096, 1, 256, 3, OPCODE_READ, OPCODE_PROG, OPCODE_WREN, OPCODE_E4K, 0, OPCODE_E64K, 0 },
};

static inline int spinor_read_sfdp(struct xfel_ctx_t * ctx, struct sfdp_t * sfdp)
{
	uint32_t addr;
	uint8_t tx[5];
	int i, r;

	memset(sfdp, 0, sizeof(struct sfdp_t));
	tx[0] = OPCODE_SFDP;
	tx[1] = 0x0;
	tx[2] = 0x0;
	tx[3] = 0x0;
	tx[4] = 0x0;
	fel_chip_spi_select(ctx);
	r = fel_chip_spi_xfer(ctx, tx, 5, &sfdp->h, sizeof(struct sfdp_header_t));
	fel_chip_spi_deselect(ctx);
	if(r < 0)
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
		fel_chip_spi_select(ctx);
		r = fel_chip_spi_xfer(ctx, tx, 5, &sfdp->ph[i], sizeof(struct sfdp_parameter_header_t));
		fel_chip_spi_deselect(ctx);
		if(r < 0)
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
			fel_chip_spi_select(ctx);
			r = fel_chip_spi_xfer(ctx, tx, 5, &sfdp->bt.table[0], sfdp->ph[i].length * 4);
			fel_chip_spi_deselect(ctx);
			if(r >= 0)
			{
				sfdp->bt.major = sfdp->ph[i].major;
				sfdp->bt.minor = sfdp->ph[i].minor;
				return 1;
			}
		}
	}
	return 0;
}

static inline int spinor_read_id(struct xfel_ctx_t * ctx, uint32_t * id)
{
	uint8_t tx[1];
	uint8_t rx[3];
	int r;

	tx[0] = OPCODE_RDID;
	fel_chip_spi_select(ctx);
	r = fel_chip_spi_xfer(ctx, tx, 1, rx, 3);
	fel_chip_spi_deselect(ctx);
	if(r < 0)
		return 0;
	*id = (rx[0] << 16) | (rx[1] << 8) | (rx[2] << 0);
	return 1;
}

static inline int spinor_info(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	const struct spinor_info_t * t;
	struct sfdp_t sfdp;
	uint32_t v, id;
	int i;

	if(spinor_read_sfdp(ctx, &sfdp))
	{
		info->name = "";
		info->id = 0;
		/* Basic flash parameter table 2th dword */
		v = (sfdp.bt.table[7] << 24) | (sfdp.bt.table[6] << 16) | (sfdp.bt.table[5] << 8) | (sfdp.bt.table[4] << 0);
		if(v & (1 << 31))
		{
			v &= 0x7fffffff;
			info->capacity = 1 << (v - 3);
		}
		else
		{
			info->capacity = (v + 1) >> 3;
		}
		/* Basic flash parameter table 1th dword */
		v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
		if((info->capacity <= (16 * 1024 * 1024)) && (((v >> 17) & 0x3) != 0x2))
			info->address_length = 3;
		else
			info->address_length = 4;
		if(((v >> 0) & 0x3) == 0x1)
			info->opcode_erase_4k = (v >> 8) & 0xff;
		else
			info->opcode_erase_4k = 0x00;
		info->opcode_erase_32k = 0x00;
		info->opcode_erase_64k = 0x00;
		info->opcode_erase_256k = 0x00;
		/* Basic flash parameter table 8th dword */
		v = (sfdp.bt.table[31] << 24) | (sfdp.bt.table[30] << 16) | (sfdp.bt.table[29] << 8) | (sfdp.bt.table[28] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		/* Basic flash parameter table 9th dword */
		v = (sfdp.bt.table[35] << 24) | (sfdp.bt.table[34] << 16) | (sfdp.bt.table[33] << 8) | (sfdp.bt.table[32] << 0);
		switch((v >> 0) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 8) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 8) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 8) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 8) & 0xff;
			break;
		default:
			break;
		}
		switch((v >> 16) & 0xff)
		{
		case 12:
			info->opcode_erase_4k = (v >> 24) & 0xff;
			break;
		case 15:
			info->opcode_erase_32k = (v >> 24) & 0xff;
			break;
		case 16:
			info->opcode_erase_64k = (v >> 24) & 0xff;
			break;
		case 18:
			info->opcode_erase_256k = (v >> 24) & 0xff;
			break;
		default:
			break;
		}
		if(info->opcode_erase_4k != 0x00)
			info->blksz = 4096;
		else if(info->opcode_erase_32k != 0x00)
			info->blksz = 32768;
		else if(info->opcode_erase_64k != 0x00)
			info->blksz = 65536;
		else if(info->opcode_erase_256k != 0x00)
			info->blksz = 262144;
		info->opcode_write_enable = OPCODE_WREN;
		info->read_granularity = 1;
		info->opcode_read = OPCODE_READ;
		if((sfdp.bt.major == 1) && (sfdp.bt.minor < 5))
		{
			/* Basic flash parameter table 1th dword */
			v = (sfdp.bt.table[3] << 24) | (sfdp.bt.table[2] << 16) | (sfdp.bt.table[1] << 8) | (sfdp.bt.table[0] << 0);
			if((v >> 2) & 0x1)
				info->write_granularity = 64;
			else
				info->write_granularity = 1;
		}
		else if((sfdp.bt.major == 1) && (sfdp.bt.minor >= 5))
		{
			/* Basic flash parameter table 11th dword */
			v = (sfdp.bt.table[43] << 24) | (sfdp.bt.table[42] << 16) | (sfdp.bt.table[41] << 8) | (sfdp.bt.table[40] << 0);
			info->write_granularity = 1 << ((v >> 4) & 0xf);
		}
		info->opcode_write = OPCODE_PROG;
		return 1;
	}
	else if(spinor_read_id(ctx, &id) && (id != 0xffffff) && (id != 0))
	{
		for(i = 0; i < ARRAY_SIZE(spinor_infos); i++)
		{
			t = &spinor_infos[i];
			if(id == t->id)
			{
				memcpy(info, t, sizeof(struct spinor_info_t));
				return 1;
			}
		}
		printf("The spi nor flash '0x%x' is not yet supported", id);
	}
	return 0;
}

static inline uint8_t spinor_read_status_register(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	uint8_t tx = OPCODE_RDSR;
	uint8_t rx = 0;

	fel_chip_spi_select(ctx);
	fel_chip_spi_xfer(ctx, &tx, 1, &rx, 1);
	fel_chip_spi_deselect(ctx);
	return rx;
}

static inline void spinor_write_status_register(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint8_t sr)
{
	uint8_t tx[2];

	tx[0] = OPCODE_WRSR;
	tx[1] = sr;
	fel_chip_spi_select(ctx);
	fel_chip_spi_xfer(ctx, tx, 2, 0, 0);
	fel_chip_spi_deselect(ctx);
}

static inline void spinor_write_enable(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	fel_chip_spi_select(ctx);
	fel_chip_spi_xfer(ctx, &info->opcode_write_enable, 1, 0, 0);
	fel_chip_spi_deselect(ctx);
}

static inline void spinor_address_mode_4byte(struct xfel_ctx_t * ctx, struct spinor_info_t * info, int enable)
{
	uint8_t tx;

	if(enable)
		tx = OPCODE_ENTER_4B;
	else
		tx = OPCODE_EXIT_4B;
	fel_chip_spi_select(ctx);
	fel_chip_spi_xfer(ctx, &tx, 1, 0, 0);
	fel_chip_spi_deselect(ctx);
}

static inline void spinor_chip_reset(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	uint8_t tx[2];

	tx[0] = 0x66;
	tx[1] = 0x99;
	fel_chip_spi_select(ctx);
	fel_chip_spi_xfer(ctx, tx, 2, 0, 0);
	fel_chip_spi_deselect(ctx);
}

static inline void spinor_wait_for_busy(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	while((spinor_read_status_register(ctx, info) & 0x1) == 0x1);
}

static void spinor_read_bytes(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr, uint8_t * buf, uint32_t count)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_read;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, buf, count);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_read;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, buf, count);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static void spinor_write_bytes(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr, uint8_t * buf, uint32_t count)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_write;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, 0, 0);
		fel_chip_spi_xfer(ctx, buf, count, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_write;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, 0, 0);
		fel_chip_spi_xfer(ctx, buf, count, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static void spinor_sector_erase_4k(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_erase_4k;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_erase_4k;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static void spinor_sector_erase_32k(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_erase_32k;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_erase_32k;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static void spinor_sector_erase_64k(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_erase_64k;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_erase_64k;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static void spinor_sector_erase_256k(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint32_t addr)
{
	uint8_t tx[5];

	switch(info->address_length)
	{
	case 3:
		tx[0] = info->opcode_erase_256k;
		tx[1] = (uint8_t)(addr >> 16);
		tx[2] = (uint8_t)(addr >> 8);
		tx[3] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 4, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	case 4:
		tx[0] = info->opcode_erase_256k;
		tx[1] = (uint8_t)(addr >> 24);
		tx[2] = (uint8_t)(addr >> 16);
		tx[3] = (uint8_t)(addr >> 8);
		tx[4] = (uint8_t)(addr >> 0);
		fel_chip_spi_select(ctx);
		fel_chip_spi_xfer(ctx, tx, 5, 0, 0);
		fel_chip_spi_deselect(ctx);
		break;

	default:
		break;
	}
}

static uint64_t spinor_block_read(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint8_t * buf, uint64_t blkno, uint64_t blkcnt)
{
	uint64_t addr = blkno * info->blksz;
	int64_t cnt = blkcnt * info->blksz;
	uint8_t * pbuf = buf;
	uint32_t len;

	if(info->read_granularity == 1)
		len = (cnt < 0x7fffffff) ? cnt : 0x7fffffff;
	else
		len = info->read_granularity;
	while(cnt > 0)
	{
		spinor_wait_for_busy(ctx, info);
		spinor_read_bytes(ctx, info, addr, pbuf, len);
		addr += len;
		pbuf += len;
		cnt -= len;
	}
	return blkcnt;
}

static uint64_t spinor_block_write(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint8_t * buf, uint64_t blkno, uint64_t blkcnt)
{
	uint64_t addr, baddr = blkno * info->blksz;
	int64_t cnt, count = blkcnt * info->blksz;
	uint32_t len;
	uint8_t * pbuf;

	addr = baddr;
	cnt = count;
	spinor_wait_for_busy(ctx, info);
	while(cnt > 0)
	{
		if((info->opcode_erase_256k != 0) && ((addr & 0x3ffff) == 0) && (cnt >= 262144))
		{
			len = 262144;
			spinor_write_enable(ctx, info);
			spinor_sector_erase_256k(ctx, info, addr);
			spinor_wait_for_busy(ctx, info);
		}
		else if((info->opcode_erase_64k != 0) && ((addr & 0xffff) == 0) && (cnt >= 65536))
		{
			len = 65536;
			spinor_write_enable(ctx, info);
			spinor_sector_erase_64k(ctx, info, addr);
			spinor_wait_for_busy(ctx, info);
		}
		else if((info->opcode_erase_32k != 0) && ((addr & 0x7fff) == 0) && (cnt >= 32768))
		{
			len = 32768;
			spinor_write_enable(ctx, info);
			spinor_sector_erase_32k(ctx, info, addr);
			spinor_wait_for_busy(ctx, info);
		}
		else if((info->opcode_erase_4k != 0) && ((addr & 0xfff) == 0) && (cnt >= 4096))
		{
			len = 4096;
			spinor_write_enable(ctx, info);
			spinor_sector_erase_4k(ctx, info, addr);
			spinor_wait_for_busy(ctx, info);
		}
		else
		{
			return 0;
		}
		addr += len;
		cnt -= len;
	}
	addr = baddr;
	cnt = count;
	pbuf = buf;
	if(info->write_granularity == 1)
		len = (cnt < 0x7fffffff) ? cnt : 0x7fffffff;
	else
		len = info->write_granularity;
	spinor_wait_for_busy(ctx, info);
	while(cnt > 0)
	{
		spinor_write_enable(ctx, info);
		spinor_write_bytes(ctx, info, addr, pbuf, len);
		spinor_wait_for_busy(ctx, info);
		addr += len;
		pbuf += len;
		cnt -= len;
	}
	return blkcnt;
}

static uint64_t block_write(struct xfel_ctx_t * ctx, struct spinor_info_t * info, uint64_t offset, void * buf, uint64_t count)
{
	uint64_t blkno, blksz, blkcnt, capacity;
	uint64_t len, tmp;
	uint64_t ret = 0;
	uint8_t * p;

	blksz = info->blksz;
	blkcnt = info->capacity / info->blksz;
	if(!blksz || !blkcnt)
		return 0;
	capacity = info->capacity;
	if(offset >= capacity)
		return 0;
	tmp = capacity - offset;
	if(count > tmp)
		count = tmp;
	p = malloc(blksz);
	if(!p)
		return 0;
	blkno = offset / blksz;
	tmp = offset % blksz;
	if(tmp > 0)
	{
		len = blksz - tmp;
		if(count < len)
			len = count;
		if(spinor_block_read(ctx, info, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}
		memcpy((void *)(&p[tmp]), (const void *)buf, len);
		if(spinor_block_write(ctx, info, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}
		buf += len;
		count -= len;
		ret += len;
		blkno += 1;
	}
	tmp = count / blksz;
	if(tmp > 0)
	{
		len = tmp * blksz;
		if(spinor_block_write(ctx, info, buf, blkno, tmp) != tmp)
		{
			free(p);
			return ret;
		}
		buf += len;
		count -= len;
		ret += len;
		blkno += tmp;
	}
	if(count > 0)
	{
		len = count;
		if(spinor_block_read(ctx, info, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}
		memcpy((void *)(&p[0]), (const void *)buf, len);
		if(spinor_block_write(ctx, info, p, blkno, 1) != 1)
		{
			free(p);
			return ret;
		}
		ret += len;
	}
	free(p);
	return ret;
}

static int spinor_init(struct xfel_ctx_t * ctx, struct spinor_info_t * info)
{
	fel_chip_spi_init(ctx);
	if(spinor_info(ctx, info))
	{
		spinor_chip_reset(ctx, info);
		spinor_wait_for_busy(ctx, info);
		spinor_write_enable(ctx, info);
		spinor_write_status_register(ctx, info, 0);
		spinor_wait_for_busy(ctx, info);
		if(info->address_length == 4)
		{
			spinor_write_enable(ctx, info);
			spinor_address_mode_4byte(ctx, info, 1);
			spinor_wait_for_busy(ctx, info);
		}
		return 1;
	}
	return 0;
}

uint64_t spinor_detect(struct xfel_ctx_t * ctx)
{
	struct spinor_info_t info;

	if(spinor_init(ctx, &info))
		return info.capacity;
	return 0;
}

int spinor_read(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinor_info_t info;
	struct progress_t p;
	uint64_t n;

	if(spinor_init(ctx, &info))
	{
		progress_start(&p, len);
		while(len > 0)
		{
			n = len > 65536 ? 65536 : len;
			spinor_read_bytes(ctx, &info, addr, buf, n);
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

int spinor_write(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len)
{
	struct spinor_info_t info;
	struct progress_t p;
	uint64_t n;

	if(spinor_init(ctx, &info))
	{
		progress_start(&p, len);
		while(len > 0)
		{
			n = len > 65536 ? 65536 : len;
			block_write(ctx, &info, addr, buf, n);
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
