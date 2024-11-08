#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	return 0;
}

static uint32_t sid_read_key(struct xfel_ctx_t * ctx, uint32_t addr, int offset)
{
	uint32_t val;

	val = R32(addr + 0x40);
	val &= ~(((0x1ff) << 16) | 0x3);
	val |= (offset << 16);
	W32(addr + 0x40, val);

	val &= ~(((0xff) << 8) | 0x3);
	val |= (0xac << 8) | 0x2;
	W32(addr + 0x40, val);

	while(R32(addr + 0x40) & (1 << 1));
	val &= ~(((0x1ff) << 16) | ((0xff) << 8) | 0x3);
	W32(addr + 0x40, val);

	return R32(addr + 0x60);
}

static int chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
{
	sid[0] = sid_read_key(ctx, 0x01c14000, 0 * 4);
	sid[1] = sid_read_key(ctx, 0x01c14000, 1 * 4);
	sid[2] = sid_read_key(ctx, 0x01c14000, 2 * 4);
	sid[3] = sid_read_key(ctx, 0x01c14000, 3 * 4);
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

struct chip_t h3 = {
	.name = "H3",
	.id = 0x00168000,
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
