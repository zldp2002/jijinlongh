#ifndef __SPINAND_H__
#define __SPINAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fel.h>

int spinor_detect(struct xfel_ctx_t * ctx);
size_t spinor_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
size_t spinor_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SPINAND_H__ */
