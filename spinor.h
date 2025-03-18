#ifndef __SPINAND_H__
#define __SPINAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fel.h>

uint64_t spinor_detect(struct xfel_ctx_t * ctx);
int spinor_read(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len);
int spinor_write(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SPINAND_H__ */
