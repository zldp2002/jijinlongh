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
	int (*spi_init)(struct xfel_ctx_t * ctx);
	int (*spi_exit)(struct xfel_ctx_t * ctx);
	int (*spi_select)(struct xfel_ctx_t * ctx);
	int (*spi_deselect)(struct xfel_ctx_t * ctx);
	int (*spi_xfer)(struct xfel_ctx_t * ctx, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen);
};

#define R8(reg)			fel_read8(ctx, reg)
#define W8(reg, val)	fel_write8(ctx, reg, val)
#define R16(reg)		fel_read16(ctx, reg)
#define W16(reg, val)	fel_write16(ctx, reg, val)
#define R32(reg)		fel_read32(ctx, reg)
#define W32(reg, val)	fel_write32(ctx, reg, val)

int fel_init(struct xfel_ctx_t * ctx);
void fel_exec(struct xfel_ctx_t * ctx, uint32_t addr);
uint8_t fel_read8(struct xfel_ctx_t * ctx, uint32_t addr);
void fel_write8(struct xfel_ctx_t * ctx, uint32_t addr, uint8_t val);
uint16_t fel_read16(struct xfel_ctx_t * ctx, uint32_t addr);
void fel_write16(struct xfel_ctx_t * ctx, uint32_t addr, uint16_t val);
uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr);
void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val);
void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_read_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
void fel_write_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);

int fel_chip_reset(struct xfel_ctx_t * ctx);
int fel_chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid);
int fel_chip_jtag(struct xfel_ctx_t * ctx);
int fel_chip_ddr(struct xfel_ctx_t * ctx, const char * type);
int fel_chip_spi_init(struct xfel_ctx_t * ctx);
int fel_chip_spi_exit(struct xfel_ctx_t * ctx);
int fel_chip_spi_select(struct xfel_ctx_t * ctx);
int fel_chip_spi_deselect(struct xfel_ctx_t * ctx);
int fel_chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen);

#ifdef __cplusplus
}
#endif

#endif /* __FEL_H__ */
