#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	uint32_t val;

	val = R32(0x01c20ca0 + 0x18);
	val &= ~(0xf << 4);
	val |= (1 << 4) | (0x1 << 0);
	W32(0x01c20ca0 + 0x18, val);
	W32(0x01c20ca0 + 0x10, (0xa57 << 1) | (1 << 0));
	return 1;
}

static int chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
{
	sid[0] = R32(0x01c23800 + 0x0);
	sid[1] = R32(0x01c23800 + 0x4);
	sid[2] = R32(0x01c23800 + 0x8);
	sid[3] = R32(0x01c23800 + 0xc);
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

static int chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, int txlen, void * rxbuf, int rxlen)
{
	return 0;
}

struct chip_t v3s = {
	.name = "V3S",
	.id = 0x00168100,
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
