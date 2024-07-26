#include <fel.h>

static int chip_reset(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_sid(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_jtag(struct xfel_ctx_t * ctx)
{
	return 0;
}

static int chip_ddr(struct xfel_ctx_t * ctx)
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
};
