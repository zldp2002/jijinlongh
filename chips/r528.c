#include <fel.h>

static int chip_detect(struct xfel_ctx_t * ctx, uint32_t id)
{
	if(id == 0x00185900)
	{
		/*
		 * Dual-Core ARM Cortex-A7
		 */
		if(R32(0x00000000) == 0xea000019)
			return 1;
	}
	return 0;
}

static int chip_reset(struct xfel_ctx_t * ctx)
{
	W32(0x020500a0 + 0x08, (0x16aa << 16) | (0x1 << 0));
	return 1;
}

static int chip_sid(struct xfel_ctx_t * ctx, char * sid)
{
	uint32_t id[4];

	id[0] = R32(0x03006200 + 0x0);
	id[1] = R32(0x03006200 + 0x4);
	id[2] = R32(0x03006200 + 0x8);
	id[3] = R32(0x03006200 + 0xc);
	sprintf(sid, "%08x%08x%08x%08x", id[0], id[1], id[2], id[3]);
	return 1;
}

static int chip_jtag(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_ddr(struct xfel_ctx_t * ctx, const char * type)
{
	return 0;
}

static int chip_spi_init(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen, uint32_t * cmdlen)
{
	return 0;
}

static int chip_spi_run(struct xfel_ctx_t * ctx, uint8_t * cbuf, uint32_t clen)
{
	return 0;
}

struct chip_t r528 = {
	.name = "R528",
	.detect = chip_detect,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
	.spi_init = chip_spi_init,
	.spi_run = chip_spi_run,
};
