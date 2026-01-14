/**
 * @file pool_allocator.h
 * @brief Fixed-size object pool allocator
 *
 * O(1) allocation and deallocation for fixed-size objects.
 */

#ifndef ENGINE_CORE_MEMORY_POOL_ALLOCATOR_H
#define ENGINE_CORE_MEMORY_POOL_ALLOCATOR_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define POOL_DEFAULT_CAPACITY 256

/* Pool allocator structure */
typedef struct {
    u8*     memory;         /* Pool memory block */
    u8*     free_list;      /* Free list head */
    u32     object_size;    /* Size of each object */
    u32     capacity;       /* Maximum objects */
    u32     allocated;      /* Currently allocated */
    u32     high_water;     /* Peak allocation */
} PoolAllocator;

/**
 * Initialize pool allocator
 * @param pool        Pool instance
 * @param object_size Size of each object (minimum 8 bytes)
 * @param capacity    Maximum number of objects
 * @return ENGINE_OK on success
 */
int pool_init(PoolAllocator* pool, u32 object_size, u32 capacity);

/**
 * Cleanup pool allocator
 * @param pool Pool instance
 */
void pool_cleanup(PoolAllocator* pool);

/**
 * Allocate an object from pool
 * @param pool Pool instance
 * @return Pointer to object or NULL if pool is exhausted
 */
void* pool_alloc(PoolAllocator* pool);

/**
 * Return object to pool
 * @param pool Pool instance
 * @param ptr  Object pointer (must be from this pool)
 */
void pool_free(PoolAllocator* pool, void* ptr);

/**
 * Reset pool (mark all objects as free)
 * @param pool Pool instance
 */
void pool_reset(PoolAllocator* pool);

/**
 * Check if pointer is from this pool
 * @param pool Pool instance
 * @param ptr  Pointer to check
 * @return 1 if from pool, 0 otherwise
 */
int pool_contains(const PoolAllocator* pool, const void* ptr);

/**
 * Get allocation statistics
 * @param pool       Pool instance
 * @param capacity   Output: total capacity
 * @param allocated  Output: current allocation
 * @param high_water Output: peak allocation
 */
void pool_get_stats(const PoolAllocator* pool,
                    u32* capacity, u32* allocated, u32* high_water);

/**
 * Get available count
 * @param pool Pool instance
 * @return Number of available objects
 */
u32 pool_available(const PoolAllocator* pool);

/**
 * Check if pool is empty
 * @param pool Pool instance
 * @return 1 if empty, 0 otherwise
 */
int pool_is_empty(const PoolAllocator* pool);

/**
 * Check if pool is full
 * @param pool Pool instance
 * @return 1 if full, 0 otherwise
 */
int pool_is_full(const PoolAllocator* pool);

/* Typed pool macros */
#define POOL_DEFINE(type, name, capacity) \
    static PoolAllocator name##_pool; \
    static int name##_init(void) { \
        return pool_init(&name##_pool, sizeof(type), capacity); \
    } \
    static void name##_cleanup(void) { \
        pool_cleanup(&name##_pool); \
    } \
    static type* name##_alloc(void) { \
        return (type*)pool_alloc(&name##_pool); \
    } \
    static void name##_free(type* obj) { \
        pool_free(&name##_pool, obj); \
    }

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_MEMORY_POOL_ALLOCATOR_H */
