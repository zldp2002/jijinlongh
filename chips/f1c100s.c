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

static int chip_spinor(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spinor_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	return 0;
}

static int chip_spinor_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	return 0;
}

static int chip_spinand(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_spinand_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	return 0;
}

static int chip_spinand_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
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
	.spinor = chip_spinor,
	.spinor_read = chip_spinor_read,
	.spinor_write = chip_spinor_write,
	.spinand = chip_spinand,
	.spinand_read = chip_spinand_read,
	.spinand_write = chip_spinand_write,
};
