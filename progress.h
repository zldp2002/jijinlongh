#ifndef __PROGRESS_H__
#define __PROGRESS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <x.h>

struct progress_t {
	size_t total;
	size_t done;
	double start;
};

void progress_start(struct progress_t * p, size_t total);
void progress_update(struct progress_t * p, size_t bytes);
void progress_stop(struct progress_t * p);

#ifdef __cplusplus
}
#endif

#endif /* __PROGRESS_H__ */
