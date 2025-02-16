#ifndef __SPINOR_H__
#define __SPINOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fel.h>

int spinand_detect(struct xfel_ctx_t * ctx);
size_t spinand_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);
size_t spinand_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SPINOR_H__ */
