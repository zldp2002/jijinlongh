#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
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

static int chip_spi_init(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen)
{
	return 0;
}

struct chip_t t507 = {
	.name = "T507",
	.id = 0x00182300,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
	.spi_init = chip_spi_init,
	.spi_xfer = chip_spi_xfer,
};
