#ifndef __SPINAND_H__
#define __SPINAND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <fel.h>

int spinand_detect(struct xfel_ctx_t * ctx, char * name, uint64_t * capacity);
int spinand_read(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len);
int spinand_write(struct xfel_ctx_t * ctx, uint64_t addr, void * buf, uint64_t len);
int spinand_splwrite(struct xfel_ctx_t * ctx, uint32_t splitsz, uint64_t addr, void * buf, uint64_t len);

#ifdef __cplusplus
}
#endif

#endif /* __SPINAND_H__ */
