#ifndef __FEL_H__
#define __FEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <x.h>
#include <progress.h>

struct xfel_ctx_t;
struct chip_t;

struct xfel_ctx_t {
	libusb_device_handle * hdl;
	int epout;
	int epin;
	struct {
		char magic[8];
		uint32_t id;
		uint32_t firmware;
		uint16_t protocol;
		uint8_t  dflag;
		uint8_t  dlength;
		uint32_t scratchpad;
		uint8_t pad[8];
	} version;
	struct chip_t * chip;
};

struct chip_t {
	char * name;
	uint32_t id;

	int (*reset)(struct xfel_ctx_t * ctx);
	int (*sid)(struct xfel_ctx_t * ctx, uint32_t * sid);
	int (*jtag)(struct xfel_ctx_t * ctx);
	int (*ddr)(struct xfel_ctx_t * ctx, const char * type);
	int (*spi_init)(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen);
	int (*spi_run)(struct xfel_ctx_t * ctx, uint8_t * cbuf, uint32_t clen);
};

enum {
	SPI_CMD_END				= 0x00,
	SPI_CMD_INIT			= 0x01,
	SPI_CMD_SELECT			= 0x02,
	SPI_CMD_DESELECT		= 0x03,
	SPI_CMD_FAST			= 0x04,
	SPI_CMD_TXBUF			= 0x05,
	SPI_CMD_RXBUF			= 0x06,
	SPI_CMD_SPINOR_WAIT		= 0x07,
	SPI_CMD_SPINAND_WAIT	= 0x08,
};

/*
 * This R32 and W32 macro can only be used for byte access address, Don't used for address
 * that can only support word access. Because the fel protocol can only support
 * byte operation, VERY IMPORTANT !!!
 */
#define R32(reg)		fel_read32(ctx, reg)
#define W32(reg, val)	fel_write32(ctx, reg, val)

static inline int fel_chip_reset(struct xfel_ctx_t * ctx)
{
	return ctx->chip->reset(ctx);
}

static inline int fel_chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
{
	return ctx->chip->sid(ctx, sid);
}

static inline int fel_chip_jtag(struct xfel_ctx_t * ctx)
{
	return ctx->chip->jtag(ctx);
}

static inline int fel_chip_ddr(struct xfel_ctx_t * ctx, const char * type)
{
	return ctx->chip->ddr(ctx, type);
}

static inline int fel_chip_spi_init(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen)
{
	return ctx->chip->spi_init(ctx, swapbuf, swaplen);
}

static inline int fel_chip_spi_run(struct xfel_ctx_t * ctx, uint8_t * cbuf, uint32_t clen)
{
	return ctx->chip->spi_run(ctx, cbuf, clen);
}

int fel_init(struct xfel_ctx_t * ctx);
void fel_exec(struct xfel_ctx_t * ctx, uint32_t addr);
uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr);
void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val);
void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_read_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_write_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
int fel_spi_init(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen);
int fel_spi_xfer(struct xfel_ctx_t * ctx, uint32_t swapbuf, uint32_t swaplen, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen);

#ifdef __cplusplus
}
#endif

#endif /* __FEL_H__ */
