#ifndef __FIRMWARE__INTERNAL__CACHE_H__
#define __FIRMWARE__INTERNAL__CACHE_H__

#define INVALIDATE_AREA(p, s) do {	__k1_dcache_invalidate_mem_area((__k1_uintptr_t)(void*)p, s);	\
	}while(0)

#define INVALIDATE(p) INVALIDATE_AREA((p), sizeof(*p))

#define LOAD_U32(p) ((uint32_t)__builtin_k1_lwu((void*)(&p)))
#define STORE_U32(p, val) __builtin_k1_swu((void*)&(p), (uint32_t)(val))

#define LOAD_U64(p) ((uint64_t)__builtin_k1_ldu((void*)(&p)))
#define STORE_U64(p, val) __builtin_k1_sdu((void*)&(p), (uint64_t)(val))

#define LOAD_PTR(p) ((void*)(unsigned long)(LOAD_U32(p)))
#define STORE_PTR(p, val) STORE_U32((p), (unsigned long)(val))

#endif /* __FIRMWARE__INTERNAL__CACHE_H__ */
