#include <fel.h>

static int chip_detect(struct xfel_ctx_t * ctx, uint32_t id)
{
	if(id == 0x00188600)
		return 1;
	return 0;
}

static int chip_reset(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_sid(struct xfel_ctx_t * ctx, char * sid)
{
	return 0;
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

struct chip_t v853 = {
	.name = "V853",
	.detect = chip_detect,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
	.spi_init = chip_spi_init,
	.spi_run = chip_spi_run,
};
