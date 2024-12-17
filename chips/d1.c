#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	W32(0x020500a0 + 0x08, (0x16aa << 16) | (0x1 << 0));
	return 1;
}

static int chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
{
	sid[0] = R32(0x03006200 + 0x0);
	sid[1] = R32(0x03006200 + 0x4);
	sid[2] = R32(0x03006200 + 0x8);
	sid[3] = R32(0x03006200 + 0xc);
	return 1;
}

static int chip_jtag(struct xfel_ctx_t * ctx)
{
	uint32_t addr;
	uint32_t val;

	/* Config GPIOF0, GPIOF1, GPIOF3 and GPIOF5 to RISCV-JTAG mode */
	addr = 0x020000f0 + 0x00;
	val = R32(addr);
	val &= ~(0xf << ((0 & 0x7) << 2));
	val |= ((0x4 & 0xf) << ((0 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((1 & 0x7) << 2));
	val |= ((0x4 & 0xf) << ((1 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((3 & 0x7) << 2));
	val |= ((0x4 & 0xf) << ((3 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((5 & 0x7) << 2));
	val |= ((0x4 & 0xf) << ((5 & 0x7) << 2));
	W32(addr, val);

	return 1;
}

static int chip_ddr(struct xfel_ctx_t * ctx, const char * type)
{
	return 0;
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

struct chip_t d1 = {
	.name = "D1",
	.id = 0x00185900,
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
