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

static int chip_spi_exit(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spi_select(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spi_deselect(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen)
{
	return 0;
}

struct chip_t r329 = {
	.name = "R329",
	.id = 0x00185100,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
	.spi_init = chip_spi_init,
	.spi_exit = chip_spi_exit,
	.spi_select = chip_spi_select,
	.spi_deselect = chip_spi_deselect,
	.spi_xfer = chip_spi_xfer,
};
