#include <fel.h>

static int chip_detect(struct xfel_ctx_t * ctx, uint32_t id)
{
	if(id == 0x00188300)
		return 1;
	return 0;
}

static int chip_reset(struct xfel_ctx_t * ctx)
{
	return 0;
}

/*
	.global _start
_start:
	ldr r0, _adr
	add r1, pc, #12
	ldr r2, [r0]
	str r2, [r1, #0]
	bx lr
.align 2
_adr:
	.word 0x11223344
_val:
	.word 0x55667788
*/
static uint32_t payload_thumb_read32(struct xfel_ctx_t * ctx, uint32_t addr)
{
	static const uint8_t payload[] = {
		0x02, 0x48, 0x03, 0xa1, 0x02, 0x68, 0x0a, 0x60, 0x70, 0x47, 0x00, 0xbf,
	};
	uint32_t adr = cpu_to_le32(addr);
	uint32_t val;

	fel_write(ctx, ctx->version.scratchpad, (void *)payload, sizeof(payload));
	fel_write(ctx, ctx->version.scratchpad + sizeof(payload), (void *)&adr, sizeof(adr));
	fel_exec(ctx, ctx->version.scratchpad);
	fel_read(ctx, ctx->version.scratchpad + sizeof(payload) + sizeof(adr), (void *)&val, sizeof(val));
	return le32_to_cpu(val);
}

static int chip_sid(struct xfel_ctx_t * ctx, char * sid)
{
	uint32_t id[4];

	id[0] = payload_thumb_read32(ctx, 0x4004e600 + 0x0);
	id[1] = payload_thumb_read32(ctx, 0x4004e600 + 0x4);
	id[2] = payload_thumb_read32(ctx, 0x4004e600 + 0x8);
	id[3] = payload_thumb_read32(ctx, 0x4004e600 + 0xc);
	sprintf(sid, "%08x%08x%08x%08x", id[0], id[1], id[2], id[3]);
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

static int chip_spi_init(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen, uint32_t * cmdlen)
{
	return 0;
}

static int chip_spi_run(struct xfel_ctx_t * ctx, uint8_t * cbuf, uint32_t clen)
{
	return 0;
}

struct chip_t r128 = {
	.name = "R128",
	.detect = chip_detect,
	.reset = chip_reset,
	.sid = chip_sid,
	.jtag = chip_jtag,
	.ddr = chip_ddr,
	.spi_init = chip_spi_init,
	.spi_run = chip_spi_run,
};
