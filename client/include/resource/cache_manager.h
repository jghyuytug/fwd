/**
 * @file cache_manager.h
 * @brief LRU Cache Manager for Resource Management
 *
 * Implements a Least Recently Used (LRU) cache for managing game resources.
 * Features:
 * - Configurable memory limit (default 256MB)
 * - O(1) insertion, lookup, and eviction
 * - Reference counting for resource lifetime
 * - Thread-safe operations (optional)
 * - Statistics tracking (hits, misses, evictions)
 *
 * The cache uses a hash table for O(1) lookups and a doubly-linked list
 * for O(1) LRU ordering.
 *
 * @date 2026-01-04
 */

#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ========================================================================== */
/* Constants                                                                   */
/* ========================================================================== */

/* Default cache configuration */
#define CACHE_DEFAULT_LIMIT         (256 * 1024 * 1024)  /* 256 MB */
#define CACHE_DEFAULT_BUCKETS       4096                  /* Hash table size */
#define CACHE_MAX_KEY_LENGTH        512                   /* Maximum key length */
#define CACHE_MIN_LIMIT             (1 * 1024 * 1024)     /* 1 MB minimum */
#define CACHE_MAX_LIMIT             (2048 * 1024 * 1024ULL) /* 2 GB maximum */

/* Return codes */
#define CACHE_SUCCESS               0
#define CACHE_ERROR_NOT_FOUND      -1
#define CACHE_ERROR_MEMORY         -2
#define CACHE_ERROR_INVALID        -3
#define CACHE_ERROR_FULL           -4
#define CACHE_ERROR_LOCKED         -5
#define CACHE_ERROR_NOT_INIT       -6

/* Resource type categories for statistics */
typedef enum CacheResourceType {
    CACHE_TYPE_TEXTURE = 0,
    CACHE_TYPE_SPRITE,
    CACHE_TYPE_ANIMATION,
    CACHE_TYPE_SOUND,
    CACHE_TYPE_DATA,
    CACHE_TYPE_OTHER,
    CACHE_TYPE_COUNT
} CacheResourceType;

/* ========================================================================== */
/* Data Structures                                                             */
/* ========================================================================== */

/**
 * @brief Cache entry
 */
typedef struct CacheEntry {
    /* Key */
    char                key[CACHE_MAX_KEY_LENGTH];
    uint32_t            key_hash;

    /* Value */
    void*               data;           /* Resource data pointer */
    size_t              size;           /* Resource size in bytes */
    CacheResourceType   type;           /* Resource type for stats */

    /* Lifecycle */
    uint32_t            ref_count;      /* Reference count */
    uint32_t            access_count;   /* Total access count */
    uint64_t            create_time;    /* Creation timestamp (ms) */
    uint64_t            access_time;    /* Last access timestamp (ms) */

    /* LRU list pointers */
    struct CacheEntry*  lru_prev;       /* More recently used */
    struct CacheEntry*  lru_next;       /* Less recently used */

    /* Hash bucket chain */
    struct CacheEntry*  hash_next;      /* Next in hash bucket */

    /* Flags */
    bool                pinned;         /* Cannot be evicted */
    bool                dirty;          /* Needs write-back (future) */

    /* Destructor */
    void                (*destructor)(void* data);
} CacheEntry;

/**
 * @brief Cache statistics
 */
typedef struct CacheStats {
    /* Counters */
    uint64_t            total_hits;     /* Cache hits */
    uint64_t            total_misses;   /* Cache misses */
    uint64_t            total_inserts;  /* Total insertions */
    uint64_t            total_evictions;/* Total evictions */
    uint64_t            total_removes;  /* Manual removals */

    /* Current state */
    size_t              current_size;   /* Current memory usage */
    size_t              current_limit;  /* Current memory limit */
    int                 entry_count;    /* Number of entries */
    int                 pinned_count;   /* Number of pinned entries */

    /* Per-type statistics */
    uint32_t            type_counts[CACHE_TYPE_COUNT];
    size_t              type_sizes[CACHE_TYPE_COUNT];

    /* Performance */
    float               hit_rate;       /* Hit rate (0.0 - 1.0) */
    float               fill_rate;      /* Fill rate (0.0 - 1.0) */
} CacheStats;

/**
 * @brief Cache configuration
 */
typedef struct CacheConfig {
    size_t              memory_limit;   /* Memory limit in bytes */
    int                 bucket_count;   /* Hash table bucket count */
    bool                thread_safe;    /* Enable thread safety */
    bool                auto_evict;     /* Auto-evict when full */
    float               evict_ratio;    /* Eviction target (0.0-1.0) */
} CacheConfig;

/**
 * @brief LRU Cache Manager
 */
typedef struct CacheManager {
    /* Hash table */
    CacheEntry**        buckets;        /* Hash buckets */
    int                 bucket_count;   /* Number of buckets */

    /* LRU list */
    CacheEntry*         lru_head;       /* Most recently used */
    CacheEntry*         lru_tail;       /* Least recently used */

    /* Memory management */
    size_t              current_size;   /* Current memory usage */
    size_t              memory_limit;   /* Memory limit */
    int                 entry_count;    /* Number of entries */
    int                 pinned_count;   /* Number of pinned entries */

    /* Configuration */
    bool                auto_evict;     /* Auto-evict when full */
    float               evict_ratio;    /* Eviction target ratio */

    /* Statistics */
    CacheStats          stats;          /* Statistics */

    /* Thread safety */
    void*               mutex;          /* Mutex (if thread-safe) */
    bool                thread_safe;    /* Thread safety enabled */

    /* State */
    bool                initialized;    /* Is initialized */
} CacheManager;

/* ========================================================================== */
/* Lifecycle API                                                               */
/* ========================================================================== */

/**
 * @brief Get default configuration
 *
 * @param config        Output: default configuration
 */
void Cache_GetDefaultConfig(CacheConfig* config);

/**
 * @brief Initialize cache manager
 *
 * @param cache         Cache manager to initialize
 * @param config        Configuration (NULL for defaults)
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Init(CacheManager* cache, const CacheConfig* config);

/**
 * @brief Shutdown cache manager
 *
 * Frees all entries and resources.
 *
 * @param cache         Cache manager to shutdown
 */
void Cache_Shutdown(CacheManager* cache);

/**
 * @brief Check if cache is initialized
 *
 * @param cache         Cache manager
 * @return true if initialized
 */
bool Cache_IsInitialized(CacheManager* cache);

/* ========================================================================== */
/* Entry Management API                                                        */
/* ========================================================================== */

/**
 * @brief Insert entry into cache
 *
 * If key exists, updates the existing entry.
 * May trigger eviction if cache is full.
 *
 * @param cache         Cache manager
 * @param key           Key string
 * @param data          Resource data (ownership transferred)
 * @param size          Resource size in bytes
 * @param type          Resource type
 * @param destructor    Function to free data (NULL for default free())
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Insert(CacheManager* cache, const char* key,
                 void* data, size_t size, CacheResourceType type,
                 void (*destructor)(void*));

/**
 * @brief Get entry from cache
 *
 * Moves entry to front of LRU list (marks as recently used).
 * Increments reference count.
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return Data pointer or NULL if not found
 */
void* Cache_Get(CacheManager* cache, const char* key);

/**
 * @brief Get entry without updating LRU
 *
 * Useful for checking existence without affecting eviction order.
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return Data pointer or NULL if not found
 */
void* Cache_Peek(CacheManager* cache, const char* key);

/**
 * @brief Release reference to entry
 *
 * Decrements reference count. Entry with ref_count=0 can be evicted.
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Release(CacheManager* cache, const char* key);

/**
 * @brief Release reference by data pointer
 *
 * @param cache         Cache manager
 * @param data          Data pointer
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_ReleaseByPtr(CacheManager* cache, void* data);

/**
 * @brief Remove entry from cache
 *
 * Entry must have ref_count <= 1 (only caller's reference).
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Remove(CacheManager* cache, const char* key);

/**
 * @brief Check if key exists in cache
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return true if exists
 */
bool Cache_Contains(CacheManager* cache, const char* key);

/* ========================================================================== */
/* Pin Management API                                                          */
/* ========================================================================== */

/**
 * @brief Pin entry (prevent eviction)
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Pin(CacheManager* cache, const char* key);

/**
 * @brief Unpin entry (allow eviction)
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return CACHE_SUCCESS on success
 */
int Cache_Unpin(CacheManager* cache, const char* key);

/**
 * @brief Check if entry is pinned
 *
 * @param cache         Cache manager
 * @param key           Key string
 *
 * @return true if pinned
 */
bool Cache_IsPinned(CacheManager* cache, const char* key);

/* ========================================================================== */
/* Eviction API                                                                */
/* ========================================================================== */

/**
 * @brief Evict least recently used entries
 *
 * Evicts entries with ref_count=0 until target_size is reached.
 *
 * @param cache         Cache manager
 * @param target_size   Target size to reach (0 = evict one entry)
 *
 * @return Number of entries evicted
 */
int Cache_EvictLRU(CacheManager* cache, size_t target_size);

/**
 * @brief Evict to make room for new entry
 *
 * @param cache         Cache manager
 * @param required_size Size needed
 *
 * @return Bytes freed
 */
size_t Cache_MakeRoom(CacheManager* cache, size_t required_size);

/**
 * @brief Clear all entries
 *
 * Entries with ref_count > 0 will NOT be cleared unless force=true.
 *
 * @param cache         Cache manager
 * @param force         Force clear all (including referenced)
 *
 * @return Number of entries cleared
 */
int Cache_Clear(CacheManager* cache, bool force);

/**
 * @brief Garbage collect unreferenced entries
 *
 * Removes entries with ref_count=0 that haven't been accessed recently.
 *
 * @param cache         Cache manager
 * @param max_age_ms    Maximum age in milliseconds (0 = no limit)
 *
 * @return Number of entries collected
 */
int Cache_GarbageCollect(CacheManager* cache, uint64_t max_age_ms);

/* ========================================================================== */
/* Configuration API                                                           */
/* ========================================================================== */

/**
 * @brief Set memory limit
 *
 * May trigger eviction if new limit is lower.
 *
 * @param cache         Cache manager
 * @param limit_bytes   New limit in bytes
 */
void Cache_SetLimit(CacheManager* cache, size_t limit_bytes);

/**
 * @brief Get memory limit
 *
 * @param cache         Cache manager
 * @return Current limit
 */
size_t Cache_GetLimit(CacheManager* cache);

/**
 * @brief Get current memory usage
 *
 * @param cache         Cache manager
 * @return Current usage in bytes
 */
size_t Cache_GetUsage(CacheManager* cache);

/**
 * @brief Get entry count
 *
 * @param cache         Cache manager
 * @return Number of entries
 */
int Cache_GetEntryCount(CacheManager* cache);

/* ========================================================================== */
/* Statistics API                                                              */
/* ========================================================================== */

/**
 * @brief Get cache statistics
 *
 * @param cache         Cache manager
 * @param stats         Output: statistics
 */
void Cache_GetStats(CacheManager* cache, CacheStats* stats);

/**
 * @brief Get hit rate
 *
 * @param cache         Cache manager
 * @return Hit rate (0.0 - 1.0)
 */
float Cache_GetHitRate(CacheManager* cache);

/**
 * @brief Reset statistics counters
 *
 * @param cache         Cache manager
 */
void Cache_ResetStats(CacheManager* cache);

/**
 * @brief Print cache statistics
 *
 * @param cache         Cache manager
 */
void Cache_PrintStats(CacheManager* cache);

/* ========================================================================== */
/* Iteration API                                                               */
/* ========================================================================== */

/**
 * @brief Iteration callback
 *
 * @param key           Entry key
 * @param data          Entry data
 * @param size          Entry size
 * @param ref_count     Reference count
 * @param userdata      User data
 *
 * @return false to stop iteration
 */
typedef bool (*CacheIterator)(const char* key, void* data, size_t size,
                              uint32_t ref_count, void* userdata);

/**
 * @brief Iterate over all entries
 *
 * Iteration order is LRU (most recent first).
 *
 * @param cache         Cache manager
 * @param callback      Callback function
 * @param userdata      User data for callback
 */
void Cache_Iterate(CacheManager* cache, CacheIterator callback, void* userdata);

/**
 * @brief Iterate over entries of specific type
 *
 * @param cache         Cache manager
 * @param type          Resource type to filter
 * @param callback      Callback function
 * @param userdata      User data for callback
 */
void Cache_IterateByType(CacheManager* cache, CacheResourceType type,
                         CacheIterator callback, void* userdata);

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

/**
 * @brief Compute hash for key
 *
 * @param key           Key string
 * @return Hash value
 */
uint32_t Cache_HashKey(const char* key);

/**
 * @brief Get current timestamp
 *
 * @return Timestamp in milliseconds
 */
uint64_t Cache_GetTimestamp(void);

/**
 * @brief Get type name
 *
 * @param type          Resource type
 * @return String name
 */
const char* Cache_GetTypeName(CacheResourceType type);

/**
 * @brief Format size for display
 *
 * @param size          Size in bytes
 * @param buffer        Output buffer
 * @param buffer_size   Buffer size
 * @return buffer pointer
 */
const char* Cache_FormatSize(size_t size, char* buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* CACHE_MANAGER_H */
