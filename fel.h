#ifndef __FEL_H__
#define __FEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <libusb.h>
#include <byteorder.h>

#define ARRAY_SIZE(array)	(sizeof(array) / sizeof((array)[0]))

struct fel_version_t {
	char signature[8];
	uint32_t id;
	uint32_t unknown_0a;
	uint16_t protocol;
	uint8_t  unknown_12;
	uint8_t  unknown_13;
	uint32_t scratchpad;
	uint32_t pad[2];
};

struct chip_t {
	char * name;
	uint32_t id;
};

struct xfel_ctx_t {
	libusb_device_handle * hdl;
	int epout;
	int epin;
	struct fel_version_t ver;
	struct chip_t * chip;
};

int fel_init(struct xfel_ctx_t * ctx);
void fel_read(struct xfel_ctx_t * ctx, uint32_t offset, void * buf, size_t len);
void fel_write(struct xfel_ctx_t * ctx, uint32_t offset, void * buf, size_t len);
void fel_exec(struct xfel_ctx_t * ctx, uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* __FEL_H__ */
