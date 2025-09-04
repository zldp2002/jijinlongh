#ifndef __STRING_H__
#define __STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

void * memset(void * s, int c, size_t n);
void * memcpy(void * dest, const void * src, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* __STRING_H__ */
