#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	uint32_t val;

	val = R32(0x030090a0 + 0x18);
	val &= ~(0xf << 4);
	val |= (1 << 4) | (0x1 << 0);
	W32(0x030090a0 + 0x18, val);
	W32(0x030090a0 + 0x10, (0xa57 << 1) | (1 << 0));
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

	/* Config GPIOF0, GPIOF1, GPIOF3 and GPIOF5 to JTAG mode */
	addr = 0x0300b0b4 + 0x00;
	val = R32(addr);
	val &= ~(0xf << ((0 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((0 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((1 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((1 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((3 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((3 & 0x7) << 2));
	W32(addr, val);

	val = R32(addr);
	val &= ~(0xf << ((5 & 0x7) << 2));
	val |= ((0x3 & 0x7) << ((5 & 0x7) << 2));
	W32(addr, val);

	return 1;
}

static int chip_ddr(struct xfel_ctx_t * ctx)
{
	return 0;
}

struct chip_t v831 = {
	.name = "V831",
	.id = 0x00181700,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
};
