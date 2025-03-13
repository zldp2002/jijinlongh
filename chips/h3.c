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
	static const uint32_t payload[] = {
		cpu_to_le32(0xe59f0040), /*    0:  ldr   r0, [pc, #64]           */
		cpu_to_le32(0xe3a01000), /*    4:  mov   r1, #0                  */
		cpu_to_le32(0xe28f303c), /*    8:  add   r3, pc, #60             */
		/* <sid_read_loop>: */
		cpu_to_le32(0xe1a02801), /*    c:  lsl   r2, r1, #16             */
		cpu_to_le32(0xe3822b2b), /*   10:  orr   r2, r2, #44032          */
		cpu_to_le32(0xe3822002), /*   14:  orr   r2, r2, #2              */
		cpu_to_le32(0xe5802040), /*   18:  str   r2, [r0, #64]           */
		/* <sid_read_wait>: */
		cpu_to_le32(0xe5902040), /*   1c:  ldr   r2, [r0, #64]           */
		cpu_to_le32(0xe3120002), /*   20:  tst   r2, #2                  */
		cpu_to_le32(0x1afffffc), /*   24:  bne   1c <sid_read_wait>      */
		cpu_to_le32(0xe5902060), /*   28:  ldr   r2, [r0, #96]           */
		cpu_to_le32(0xe7832001), /*   2c:  str   r2, [r3, r1]            */
		cpu_to_le32(0xe2811004), /*   30:  add   r1, r1, #4              */
		cpu_to_le32(0xe3510010), /*   34:  cmp   r1, #16                 */
		cpu_to_le32(0x3afffff3), /*   38:  bcc   c <sid_read_loop>       */
		cpu_to_le32(0xe3a02000), /*   3c:  mov   r2, #0                  */
		cpu_to_le32(0xe5802040), /*   40:  str   r2, [r0, #64]           */
		cpu_to_le32(0xe12fff1e), /*   44:  bx    lr                      */
		cpu_to_le32(0x01c14000), /* SID base addr */
		/* SID will put here */
	};
	uint32_t res[4];
	fel_write(ctx, ctx->version.scratchpad, (void *)payload, sizeof(payload));
	fel_exec(ctx, ctx->version.scratchpad);
	fel_read(ctx, ctx->version.scratchpad + sizeof(payload), (void *)res, sizeof(res));
	sid[0] = le32_to_cpu(res[0]);
	sid[1] = le32_to_cpu(res[1]);
	sid[2] = le32_to_cpu(res[2]);
	sid[3] = le32_to_cpu(res[3]);
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

static int chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen)
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
	.spi_init = chip_spi_init,
	.spi_exit = chip_spi_exit,
	.spi_select = chip_spi_select,
	.spi_deselect = chip_spi_deselect,
	.spi_xfer = chip_spi_xfer,
};
