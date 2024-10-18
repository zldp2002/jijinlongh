#ifndef __FEL_H__
#define __FEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <x.h>

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
	int (*spinor)(struct xfel_ctx_t * ctx);
	int (*spinor_read)(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
	int (*spinor_write)(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
	int (*spinand)(struct xfel_ctx_t * ctx);
	int (*spinand_read)(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
	int (*spinand_write)(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
};

#define R32(reg)		fel_read32(ctx, reg)
#define W32(reg, val)	fel_write32(ctx, reg, val)

int fel_init(struct xfel_ctx_t * ctx);
void fel_exec(struct xfel_ctx_t * ctx, uint32_t addr);
uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr);
void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val);
void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress);
void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress);

int fel_chip_reset(struct xfel_ctx_t * ctx);
int fel_chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid);
int fel_chip_jtag(struct xfel_ctx_t * ctx);
int fel_chip_ddr(struct xfel_ctx_t * ctx, const char * type);
int fel_chip_spinor(struct xfel_ctx_t * ctx);
int fel_chip_spinor_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
int fel_chip_spinor_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
int fel_chip_spinand(struct xfel_ctx_t * ctx);
int fel_chip_spinand_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
int fel_chip_spinand_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __FEL_H__ */
