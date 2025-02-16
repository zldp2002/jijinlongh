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
	return 0;
}

static int chip_jtag(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_ddr(struct xfel_ctx_t * ctx, const char * type)
{
	W32(0x01c20800 + 0x024, 0x00007222);
	W32(0x01c20800 + 0x2c0, 0x00000aaa);
	W32(0x01c20000 + 0x020, 0x90000c00);
	usleep(1);
	W32(0x01c20000 + 0x060, 0x01004000);
	W32(0x01c20000 + 0x2c0, R32(0x01c20000 + 0x2c0) & ~(0x1 << 14));
	usleep(1);
	W32(0x01c20000 + 0x2c0, R32(0x01c20000 + 0x2c0) | (0x1 << 14));
	W32(0x01c20800 + 0x2c4, 0x00010501);
	W32(0x01c01000 + 0x004, 0xb7ceedc2);
	W32(0x01c01000 + 0x008, 0x00a70008);
	W32(0x01c01000 + 0x000, 0x0001318a);
	W32(0x01c01000 + 0x00c, 0x000800c8);
	W32(0x01c01000 + 0x00c, R32(0x01c01000 + 0x00c) | (1 << 0));
	usleep(1);
	W32(0x01c01000 + 0x024, 0x00161600);
	W32(0x01c01000 + 0x010, 0x000004c0);

	return 1;
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

struct chip_t f1c100s = {
	.name = "F1C100S",
	.id = 0x00166300,
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
