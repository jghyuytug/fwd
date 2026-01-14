/**
 * @file pool_allocator.c
 * @brief Fixed-size object pool allocator implementation
 */

#include "pool_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Minimum object size (must hold a pointer for free list) */
#define MIN_OBJECT_SIZE sizeof(void*)

int pool_init(PoolAllocator* pool, u32 object_size, u32 capacity)
{
    u32 i;
    u8* ptr;
    u32 aligned_size;

    if (!pool || capacity == 0) {
        return ENGINE_ERROR_PARAM;
    }

    /* Ensure minimum size and alignment */
    aligned_size = object_size;
    if (aligned_size < MIN_OBJECT_SIZE) {
        aligned_size = MIN_OBJECT_SIZE;
    }

    /* Align to pointer size */
    aligned_size = ENGINE_ALIGN(aligned_size, sizeof(void*));

    /* Allocate memory block */
    pool->memory = (u8*)malloc(aligned_size * capacity);
    if (!pool->memory) {
        return ENGINE_ERROR_MEMORY;
    }

    pool->object_size = aligned_size;
    pool->capacity = capacity;
    pool->allocated = 0;
    pool->high_water = 0;

    /* Build free list */
    pool->free_list = pool->memory;

    for (i = 0; i < capacity - 1; i++) {
        ptr = pool->memory + (i * aligned_size);
        /* Store pointer to next free object */
        *((u8**)ptr) = ptr + aligned_size;
    }

    /* Last object points to NULL */
    ptr = pool->memory + ((capacity - 1) * aligned_size);
    *((u8**)ptr) = NULL;

    return ENGINE_OK;
}

void pool_cleanup(PoolAllocator* pool)
{
    if (!pool) {
        return;
    }

    if (pool->memory) {
        free(pool->memory);
        pool->memory = NULL;
    }

    pool->free_list = NULL;
    pool->object_size = 0;
    pool->capacity = 0;
    pool->allocated = 0;
}

void* pool_alloc(PoolAllocator* pool)
{
    void* obj;

    if (!pool || !pool->free_list) {
        return NULL;
    }

    /* Get from free list */
    obj = pool->free_list;
    pool->free_list = *((u8**)pool->free_list);

    pool->allocated++;

    if (pool->allocated > pool->high_water) {
        pool->high_water = pool->allocated;
    }

    /* Zero the memory */
    memset(obj, 0, pool->object_size);

    return obj;
}

void pool_free(PoolAllocator* pool, void* ptr)
{
    if (!pool || !ptr) {
        return;
    }

    /* Verify pointer is from this pool */
    if (!pool_contains(pool, ptr)) {
        fprintf(stderr, "[POOL] Warning: freeing pointer not from this pool\n");
        return;
    }

    /* Add to free list */
    *((u8**)ptr) = pool->free_list;
    pool->free_list = (u8*)ptr;

    if (pool->allocated > 0) {
        pool->allocated--;
    }
}

void pool_reset(PoolAllocator* pool)
{
    u32 i;
    u8* ptr;

    if (!pool || !pool->memory) {
        return;
    }

    /* Rebuild free list */
    pool->free_list = pool->memory;

    for (i = 0; i < pool->capacity - 1; i++) {
        ptr = pool->memory + (i * pool->object_size);
        *((u8**)ptr) = ptr + pool->object_size;
    }

    ptr = pool->memory + ((pool->capacity - 1) * pool->object_size);
    *((u8**)ptr) = NULL;

    pool->allocated = 0;
}

int pool_contains(const PoolAllocator* pool, const void* ptr)
{
    const u8* p;
    u32 offset;

    if (!pool || !ptr || !pool->memory) {
        return 0;
    }

    p = (const u8*)ptr;

    /* Check if within memory block */
    if (p < pool->memory ||
        p >= pool->memory + (pool->capacity * pool->object_size)) {
        return 0;
    }

    /* Check alignment */
    offset = (u32)(p - pool->memory);
    if (offset % pool->object_size != 0) {
        return 0;
    }

    return 1;
}

void pool_get_stats(const PoolAllocator* pool,
                    u32* capacity, u32* allocated, u32* high_water)
{
    if (!pool) {
        if (capacity) *capacity = 0;
        if (allocated) *allocated = 0;
        if (high_water) *high_water = 0;
        return;
    }

    if (capacity) *capacity = pool->capacity;
    if (allocated) *allocated = pool->allocated;
    if (high_water) *high_water = pool->high_water;
}

u32 pool_available(const PoolAllocator* pool)
{
    return pool ? (pool->capacity - pool->allocated) : 0;
}

int pool_is_empty(const PoolAllocator* pool)
{
    return pool ? (pool->allocated == 0) : 1;
}

int pool_is_full(const PoolAllocator* pool)
{
    return pool ? (pool->allocated >= pool->capacity) : 1;
}
