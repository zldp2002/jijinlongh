#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline uint16_t __swab16(uint16_t x)
{
	return ( (x<<8) | (x>>8) );
}

static inline uint32_t __swab32(uint32_t x)
{
	return ( (x<<24) | (x>>24) | \
		((x & (uint32_t)0x0000ff00UL)<<8) | \
		((x & (uint32_t)0x00ff0000UL)>>8) );
}

static inline uint64_t __swab64(uint64_t x)
{
	return ( (x<<56) | (x>>56) | \
		((x & (uint64_t)0x000000000000ff00ULL)<<40) | \
		((x & (uint64_t)0x0000000000ff0000ULL)<<24) | \
		((x & (uint64_t)0x00000000ff000000ULL)<< 8) | \
		((x & (uint64_t)0x000000ff00000000ULL)>> 8) | \
		((x & (uint64_t)0x0000ff0000000000ULL)>>24) | \
		((x & (uint64_t)0x00ff000000000000ULL)>>40) );
}

static inline uint32_t __swahw32(uint32_t x)
{
	return ( ((x & (uint32_t)0x0000ffffUL)<<16) | ((x & (uint32_t)0xffff0000UL)>>16) );
}

static inline uint32_t __swahb32(uint32_t x)
{
	return ( ((x & (uint32_t)0x00ff00ffUL)<<8) | ((x & (uint32_t)0xff00ff00UL)>>8) );
}

#if (0)	/* BIG_ENDIAN */
#define cpu_to_le64(x)	(__swab64((uint64_t)(x)))
#define le64_to_cpu(x)	(__swab64((uint64_t)(x)))
#define cpu_to_le32(x)	(__swab32((uint32_t)(x)))
#define le32_to_cpu(x)	(__swab32((uint32_t)(x)))
#define cpu_to_le16(x)	(__swab16((uint16_t)(x)))
#define le16_to_cpu(x)	(__swab16((uint16_t)(x)))
#define cpu_to_be64(x)	((uint64_t)(x))
#define be64_to_cpu(x)	((uint64_t)(x))
#define cpu_to_be32(x)	((uint32_t)(x))
#define be32_to_cpu(x)	((uint32_t)(x))
#define cpu_to_be16(x)	((uint16_t)(x))
#define be16_to_cpu(x)	((uint16_t)(x))
#else
#define cpu_to_le64(x)	((uint64_t)(x))
#define le64_to_cpu(x)	((uint64_t)(x))
#define cpu_to_le32(x)	((uint32_t)(x))
#define le32_to_cpu(x)	((uint32_t)(x))
#define cpu_to_le16(x)	((uint16_t)(x))
#define le16_to_cpu(x)	((uint16_t)(x))
#define cpu_to_be64(x)	(__swab64((uint64_t)(x)))
#define be64_to_cpu(x)	(__swab64((uint64_t)(x)))
#define cpu_to_be32(x)	(__swab32((uint32_t)(x)))
#define be32_to_cpu(x)	(__swab32((uint32_t)(x)))
#define cpu_to_be16(x)	(__swab16((uint16_t)(x)))
#define be16_to_cpu(x)	(__swab16((uint16_t)(x)))
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BYTEORDER_H__ */
