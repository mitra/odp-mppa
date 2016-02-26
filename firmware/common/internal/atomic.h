#ifndef __FIRMWARE__INTERNAL__ATOMIC_H__
#define __FIRMWARE__INTERNAL__ATOMIC_H__

#include "internal/cache.h"

/**
 * @internal
 * Atomic 64-bit unsigned integer
 */
struct odp_atomic_u64_s {
	union {
		uint64_t v;
		uint64_t _type;
		uint64_t _u64;
	};
} __attribute__((aligned(sizeof(uint64_t)))); /* Enforce alignement! */

/**
 * @internal
 * Atomic 32-bit unsigned integer
 */
struct odp_atomic_u32_s {
	union {
		struct {
			uint32_t v; /**< Actual storage for the atomic variable */
			uint32_t _dummy; /**< Dummy field for force struct to 64b */
		};
		uint32_t _type;
		uint64_t _u64;
	};
} __attribute__((aligned(sizeof(uint32_t)))); /* Enforce alignement! */

/** @addtogroup odp_synchronizers
 *  @{
 */

typedef struct odp_atomic_u64_s odp_atomic_u64_t;

typedef struct odp_atomic_u32_s odp_atomic_u32_t;

static inline uint32_t odp_atomic_load_u32(odp_atomic_u32_t *atom)
{
	return LOAD_U32(atom->v);

}

static inline void odp_atomic_store_u32(odp_atomic_u32_t *atom,
					uint32_t val)
{
	return STORE_U32(atom->v, val);
}

#endif /* __FIRMWARE__INTERNAL__ATOMIC_H__ */
