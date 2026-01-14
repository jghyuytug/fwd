/**
 * @file cache_manager.c
 * @brief LRU Cache Manager Implementation
 *
 * Implements a hash table + doubly-linked list LRU cache.
 *
 * Data structure:
 * - Hash table: O(1) key lookup
 * - Doubly-linked list: O(1) LRU ordering
 *
 * Operations:
 * - Insert: O(1) average, O(n) worst case (hash collision)
 * - Lookup: O(1) average
 * - Evict:  O(1)
 * - Remove: O(1) average
 *
 * @date 2026-01-04
 */

#include "resource/cache_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/* Platform-specific includes */
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <pthread.h>
#endif

/* ========================================================================== */
/* Internal Helper Functions                                                   */
/* ========================================================================== */

/**
 * @brief Get current timestamp in milliseconds
 */
uint64_t Cache_GetTimestamp(void)
{
#ifdef _WIN32
    FILETIME ft;
    ULARGE_INTEGER li;
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    /* Convert 100-nanosecond intervals to milliseconds */
    return (li.QuadPart / 10000) - 11644473600000ULL;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

/**
 * @brief DJB2 hash function
 */
uint32_t Cache_HashKey(const char* key)
{
    uint32_t hash = 5381;
    int c;

    while ((c = *key++) != 0) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/**
 * @brief Find entry by key (internal)
 */
static CacheEntry* cache_find_entry(CacheManager* cache, const char* key)
{
    uint32_t hash;
    int bucket;
    CacheEntry* entry;

    hash = Cache_HashKey(key);
    bucket = hash % cache->bucket_count;

    entry = cache->buckets[bucket];
    while (entry != NULL) {
        if (entry->key_hash == hash && strcmp(entry->key, key) == 0) {
            return entry;
        }
        entry = entry->hash_next;
    }

    return NULL;
}

/**
 * @brief Find entry by data pointer (internal)
 */
static CacheEntry* cache_find_by_ptr(CacheManager* cache, void* data)
{
    CacheEntry* entry;

    entry = cache->lru_head;
    while (entry != NULL) {
        if (entry->data == data) {
            return entry;
        }
        entry = entry->lru_next;
    }

    return NULL;
}

/**
 * @brief Move entry to front of LRU list
 */
static void cache_touch(CacheManager* cache, CacheEntry* entry)
{
    if (entry == NULL || entry == cache->lru_head) {
        return;
    }

    /* Remove from current position */
    if (entry->lru_prev != NULL) {
        entry->lru_prev->lru_next = entry->lru_next;
    }
    if (entry->lru_next != NULL) {
        entry->lru_next->lru_prev = entry->lru_prev;
    }
    if (entry == cache->lru_tail) {
        cache->lru_tail = entry->lru_prev;
    }

    /* Insert at head */
    entry->lru_prev = NULL;
    entry->lru_next = cache->lru_head;
    if (cache->lru_head != NULL) {
        cache->lru_head->lru_prev = entry;
    }
    cache->lru_head = entry;

    if (cache->lru_tail == NULL) {
        cache->lru_tail = entry;
    }

    /* Update access time */
    entry->access_time = Cache_GetTimestamp();
    entry->access_count++;
}

/**
 * @brief Remove entry from hash table
 */
static void cache_unhash(CacheManager* cache, CacheEntry* entry)
{
    int bucket = entry->key_hash % cache->bucket_count;
    CacheEntry* prev = NULL;
    CacheEntry* curr = cache->buckets[bucket];

    while (curr != NULL) {
        if (curr == entry) {
            if (prev == NULL) {
                cache->buckets[bucket] = curr->hash_next;
            } else {
                prev->hash_next = curr->hash_next;
            }
            break;
        }
        prev = curr;
        curr = curr->hash_next;
    }
}

/**
 * @brief Remove entry from LRU list
 */
static void cache_unlist(CacheManager* cache, CacheEntry* entry)
{
    if (entry->lru_prev != NULL) {
        entry->lru_prev->lru_next = entry->lru_next;
    } else {
        cache->lru_head = entry->lru_next;
    }

    if (entry->lru_next != NULL) {
        entry->lru_next->lru_prev = entry->lru_prev;
    } else {
        cache->lru_tail = entry->lru_prev;
    }
}

/**
 * @brief Free entry and its data
 */
static void cache_free_entry(CacheEntry* entry)
{
    if (entry == NULL) {
        return;
    }

    if (entry->data != NULL) {
        if (entry->destructor != NULL) {
            entry->destructor(entry->data);
        } else {
            free(entry->data);
        }
    }

    free(entry);
}

/**
 * @brief Evict a single entry (LRU tail)
 */
static CacheEntry* cache_evict_one(CacheManager* cache)
{
    CacheEntry* entry = cache->lru_tail;

    /* Find evictable entry (ref_count=0, not pinned) */
    while (entry != NULL) {
        if (entry->ref_count == 0 && !entry->pinned) {
            break;
        }
        entry = entry->lru_prev;
    }

    if (entry == NULL) {
        return NULL; /* Nothing to evict */
    }

    /* Remove from structures */
    cache_unhash(cache, entry);
    cache_unlist(cache, entry);

    /* Update stats */
    cache->current_size -= entry->size;
    cache->entry_count--;
    cache->stats.total_evictions++;
    cache->stats.type_counts[entry->type]--;
    cache->stats.type_sizes[entry->type] -= entry->size;

    return entry;
}

/* ========================================================================== */
/* Lifecycle API Implementation                                                */
/* ========================================================================== */

void Cache_GetDefaultConfig(CacheConfig* config)
{
    if (config == NULL) {
        return;
    }

    config->memory_limit = CACHE_DEFAULT_LIMIT;
    config->bucket_count = CACHE_DEFAULT_BUCKETS;
    config->thread_safe = false;
    config->auto_evict = true;
    config->evict_ratio = 0.9f; /* Evict to 90% when full */
}

int Cache_Init(CacheManager* cache, const CacheConfig* config)
{
    CacheConfig default_config;
    int i;

    if (cache == NULL) {
        return CACHE_ERROR_INVALID;
    }

    /* Clear structure */
    memset(cache, 0, sizeof(CacheManager));

    /* Use default config if not provided */
    if (config == NULL) {
        Cache_GetDefaultConfig(&default_config);
        config = &default_config;
    }

    /* Validate limits */
    cache->memory_limit = config->memory_limit;
    if (cache->memory_limit < CACHE_MIN_LIMIT) {
        cache->memory_limit = CACHE_MIN_LIMIT;
    }
    if (cache->memory_limit > CACHE_MAX_LIMIT) {
        cache->memory_limit = CACHE_MAX_LIMIT;
    }

    cache->bucket_count = config->bucket_count;
    if (cache->bucket_count < 64) {
        cache->bucket_count = 64;
    }

    cache->auto_evict = config->auto_evict;
    cache->evict_ratio = config->evict_ratio;
    if (cache->evict_ratio < 0.5f) {
        cache->evict_ratio = 0.5f;
    }
    if (cache->evict_ratio > 0.99f) {
        cache->evict_ratio = 0.99f;
    }

    /* Allocate hash table */
    cache->buckets = (CacheEntry**)calloc(cache->bucket_count,
                                          sizeof(CacheEntry*));
    if (cache->buckets == NULL) {
        return CACHE_ERROR_MEMORY;
    }

    /* Initialize stats */
    cache->stats.current_limit = cache->memory_limit;

    /* Thread safety (placeholder - not implemented) */
    cache->thread_safe = config->thread_safe;
    cache->mutex = NULL;

    cache->initialized = true;

    return CACHE_SUCCESS;
}

void Cache_Shutdown(CacheManager* cache)
{
    if (cache == NULL || !cache->initialized) {
        return;
    }

    /* Clear all entries */
    Cache_Clear(cache, true);

    /* Free hash table */
    if (cache->buckets != NULL) {
        free(cache->buckets);
        cache->buckets = NULL;
    }

    /* Free mutex (if any) */
    if (cache->mutex != NULL) {
        /* pthread_mutex_destroy / DeleteCriticalSection */
        free(cache->mutex);
        cache->mutex = NULL;
    }

    cache->initialized = false;
}

bool Cache_IsInitialized(CacheManager* cache)
{
    return cache != NULL && cache->initialized;
}

/* ========================================================================== */
/* Entry Management API Implementation                                         */
/* ========================================================================== */

int Cache_Insert(CacheManager* cache, const char* key,
                 void* data, size_t size, CacheResourceType type,
                 void (*destructor)(void*))
{
    CacheEntry* entry;
    CacheEntry* existing;
    uint32_t hash;
    int bucket;

    if (cache == NULL || !cache->initialized) {
        return CACHE_ERROR_NOT_INIT;
    }

    if (key == NULL || data == NULL || size == 0) {
        return CACHE_ERROR_INVALID;
    }

    if (strlen(key) >= CACHE_MAX_KEY_LENGTH) {
        return CACHE_ERROR_INVALID;
    }

    /* Check if key exists */
    existing = cache_find_entry(cache, key);
    if (existing != NULL) {
        /* Update existing entry */
        if (existing->data != data) {
            /* Free old data */
            if (existing->destructor != NULL) {
                existing->destructor(existing->data);
            } else {
                free(existing->data);
            }

            /* Update size stats */
            cache->current_size -= existing->size;
            cache->stats.type_sizes[existing->type] -= existing->size;

            existing->data = data;
            existing->size = size;
            existing->destructor = destructor;

            cache->current_size += size;
            cache->stats.type_sizes[type] += size;
        }

        existing->type = type;
        cache_touch(cache, existing);
        return CACHE_SUCCESS;
    }

    /* Make room if needed */
    if (cache->auto_evict && cache->current_size + size > cache->memory_limit) {
        Cache_MakeRoom(cache, size);
    }

    /* Check if still too big */
    if (cache->current_size + size > cache->memory_limit) {
        return CACHE_ERROR_FULL;
    }

    /* Create new entry */
    entry = (CacheEntry*)calloc(1, sizeof(CacheEntry));
    if (entry == NULL) {
        return CACHE_ERROR_MEMORY;
    }

    strncpy(entry->key, key, CACHE_MAX_KEY_LENGTH - 1);
    entry->key_hash = Cache_HashKey(key);
    entry->data = data;
    entry->size = size;
    entry->type = type;
    entry->destructor = destructor;
    entry->ref_count = 0;
    entry->access_count = 1;
    entry->create_time = Cache_GetTimestamp();
    entry->access_time = entry->create_time;
    entry->pinned = false;
    entry->dirty = false;

    /* Insert into hash table */
    hash = entry->key_hash;
    bucket = hash % cache->bucket_count;
    entry->hash_next = cache->buckets[bucket];
    cache->buckets[bucket] = entry;

    /* Insert at head of LRU list */
    entry->lru_prev = NULL;
    entry->lru_next = cache->lru_head;
    if (cache->lru_head != NULL) {
        cache->lru_head->lru_prev = entry;
    }
    cache->lru_head = entry;
    if (cache->lru_tail == NULL) {
        cache->lru_tail = entry;
    }

    /* Update stats */
    cache->current_size += size;
    cache->entry_count++;
    cache->stats.total_inserts++;
    cache->stats.type_counts[type]++;
    cache->stats.type_sizes[type] += size;

    return CACHE_SUCCESS;
}

void* Cache_Get(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return NULL;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        cache->stats.total_misses++;
        return NULL;
    }

    cache->stats.total_hits++;
    entry->ref_count++;
    cache_touch(cache, entry);

    return entry->data;
}

void* Cache_Peek(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return NULL;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return NULL;
    }

    return entry->data;
}

int Cache_Release(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return CACHE_ERROR_INVALID;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return CACHE_ERROR_NOT_FOUND;
    }

    if (entry->ref_count > 0) {
        entry->ref_count--;
    }

    return CACHE_SUCCESS;
}

int Cache_ReleaseByPtr(CacheManager* cache, void* data)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || data == NULL) {
        return CACHE_ERROR_INVALID;
    }

    entry = cache_find_by_ptr(cache, data);
    if (entry == NULL) {
        return CACHE_ERROR_NOT_FOUND;
    }

    if (entry->ref_count > 0) {
        entry->ref_count--;
    }

    return CACHE_SUCCESS;
}

int Cache_Remove(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return CACHE_ERROR_INVALID;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return CACHE_ERROR_NOT_FOUND;
    }

    /* Check if in use */
    if (entry->ref_count > 1) {
        return CACHE_ERROR_LOCKED;
    }

    /* Remove from structures */
    cache_unhash(cache, entry);
    cache_unlist(cache, entry);

    /* Update stats */
    cache->current_size -= entry->size;
    cache->entry_count--;
    if (entry->pinned) {
        cache->pinned_count--;
    }
    cache->stats.total_removes++;
    cache->stats.type_counts[entry->type]--;
    cache->stats.type_sizes[entry->type] -= entry->size;

    /* Free entry */
    cache_free_entry(entry);

    return CACHE_SUCCESS;
}

bool Cache_Contains(CacheManager* cache, const char* key)
{
    if (cache == NULL || !cache->initialized || key == NULL) {
        return false;
    }

    return cache_find_entry(cache, key) != NULL;
}

/* ========================================================================== */
/* Pin Management API Implementation                                           */
/* ========================================================================== */

int Cache_Pin(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return CACHE_ERROR_INVALID;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return CACHE_ERROR_NOT_FOUND;
    }

    if (!entry->pinned) {
        entry->pinned = true;
        cache->pinned_count++;
    }

    return CACHE_SUCCESS;
}

int Cache_Unpin(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return CACHE_ERROR_INVALID;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return CACHE_ERROR_NOT_FOUND;
    }

    if (entry->pinned) {
        entry->pinned = false;
        cache->pinned_count--;
    }

    return CACHE_SUCCESS;
}

bool Cache_IsPinned(CacheManager* cache, const char* key)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || key == NULL) {
        return false;
    }

    entry = cache_find_entry(cache, key);
    if (entry == NULL) {
        return false;
    }

    return entry->pinned;
}

/* ========================================================================== */
/* Eviction API Implementation                                                 */
/* ========================================================================== */

int Cache_EvictLRU(CacheManager* cache, size_t target_size)
{
    int count = 0;
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized) {
        return 0;
    }

    if (target_size == 0) {
        /* Evict one entry */
        entry = cache_evict_one(cache);
        if (entry != NULL) {
            cache_free_entry(entry);
            count = 1;
        }
    } else {
        /* Evict until target size */
        while (cache->current_size > target_size) {
            entry = cache_evict_one(cache);
            if (entry == NULL) {
                break; /* Nothing more to evict */
            }
            cache_free_entry(entry);
            count++;
        }
    }

    return count;
}

size_t Cache_MakeRoom(CacheManager* cache, size_t required_size)
{
    size_t target_size;
    size_t freed = 0;
    size_t start_size;
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized) {
        return 0;
    }

    /* Calculate target size */
    target_size = (size_t)(cache->memory_limit * cache->evict_ratio);
    if (target_size + required_size > cache->memory_limit) {
        target_size = cache->memory_limit - required_size;
    }

    start_size = cache->current_size;

    /* Evict until target */
    while (cache->current_size > target_size) {
        entry = cache_evict_one(cache);
        if (entry == NULL) {
            break;
        }
        cache_free_entry(entry);
    }

    freed = start_size - cache->current_size;
    return freed;
}

int Cache_Clear(CacheManager* cache, bool force)
{
    int count = 0;
    CacheEntry* entry;
    CacheEntry* next;
    int i;

    if (cache == NULL || !cache->initialized) {
        return 0;
    }

    entry = cache->lru_head;
    while (entry != NULL) {
        next = entry->lru_next;

        if (force || (entry->ref_count == 0 && !entry->pinned)) {
            /* Remove from hash table */
            cache_unhash(cache, entry);

            /* Update stats */
            cache->current_size -= entry->size;
            cache->entry_count--;
            if (entry->pinned) {
                cache->pinned_count--;
            }
            cache->stats.type_counts[entry->type]--;
            cache->stats.type_sizes[entry->type] -= entry->size;

            /* Free entry */
            cache_free_entry(entry);
            count++;
        }

        entry = next;
    }

    /* Rebuild LRU list if force clear */
    if (force) {
        cache->lru_head = NULL;
        cache->lru_tail = NULL;
    } else {
        /* Rebuild LRU list from remaining entries */
        cache->lru_head = NULL;
        cache->lru_tail = NULL;

        for (i = 0; i < cache->bucket_count; i++) {
            entry = cache->buckets[i];
            while (entry != NULL) {
                entry->lru_prev = cache->lru_tail;
                entry->lru_next = NULL;
                if (cache->lru_tail != NULL) {
                    cache->lru_tail->lru_next = entry;
                } else {
                    cache->lru_head = entry;
                }
                cache->lru_tail = entry;
                entry = entry->hash_next;
            }
        }
    }

    return count;
}

int Cache_GarbageCollect(CacheManager* cache, uint64_t max_age_ms)
{
    int count = 0;
    uint64_t now;
    CacheEntry* entry;
    CacheEntry* next;

    if (cache == NULL || !cache->initialized) {
        return 0;
    }

    now = Cache_GetTimestamp();

    entry = cache->lru_tail;
    while (entry != NULL) {
        next = entry->lru_prev;

        if (entry->ref_count == 0 && !entry->pinned) {
            if (max_age_ms == 0 || (now - entry->access_time) >= max_age_ms) {
                cache_unhash(cache, entry);
                cache_unlist(cache, entry);

                cache->current_size -= entry->size;
                cache->entry_count--;
                cache->stats.total_evictions++;
                cache->stats.type_counts[entry->type]--;
                cache->stats.type_sizes[entry->type] -= entry->size;

                cache_free_entry(entry);
                count++;
            }
        }

        entry = next;
    }

    return count;
}

/* ========================================================================== */
/* Configuration API Implementation                                            */
/* ========================================================================== */

void Cache_SetLimit(CacheManager* cache, size_t limit_bytes)
{
    if (cache == NULL || !cache->initialized) {
        return;
    }

    if (limit_bytes < CACHE_MIN_LIMIT) {
        limit_bytes = CACHE_MIN_LIMIT;
    }
    if (limit_bytes > CACHE_MAX_LIMIT) {
        limit_bytes = CACHE_MAX_LIMIT;
    }

    cache->memory_limit = limit_bytes;
    cache->stats.current_limit = limit_bytes;

    /* Evict if over new limit */
    if (cache->auto_evict && cache->current_size > limit_bytes) {
        Cache_EvictLRU(cache, limit_bytes);
    }
}

size_t Cache_GetLimit(CacheManager* cache)
{
    if (cache == NULL || !cache->initialized) {
        return 0;
    }
    return cache->memory_limit;
}

size_t Cache_GetUsage(CacheManager* cache)
{
    if (cache == NULL || !cache->initialized) {
        return 0;
    }
    return cache->current_size;
}

int Cache_GetEntryCount(CacheManager* cache)
{
    if (cache == NULL || !cache->initialized) {
        return 0;
    }
    return cache->entry_count;
}

/* ========================================================================== */
/* Statistics API Implementation                                               */
/* ========================================================================== */

void Cache_GetStats(CacheManager* cache, CacheStats* stats)
{
    int i;

    if (cache == NULL || stats == NULL) {
        return;
    }

    if (!cache->initialized) {
        memset(stats, 0, sizeof(CacheStats));
        return;
    }

    /* Copy stats */
    *stats = cache->stats;

    /* Update current values */
    stats->current_size = cache->current_size;
    stats->current_limit = cache->memory_limit;
    stats->entry_count = cache->entry_count;
    stats->pinned_count = cache->pinned_count;

    /* Calculate rates */
    if (stats->total_hits + stats->total_misses > 0) {
        stats->hit_rate = (float)stats->total_hits /
                          (float)(stats->total_hits + stats->total_misses);
    } else {
        stats->hit_rate = 0.0f;
    }

    if (cache->memory_limit > 0) {
        stats->fill_rate = (float)cache->current_size / (float)cache->memory_limit;
    } else {
        stats->fill_rate = 0.0f;
    }
}

float Cache_GetHitRate(CacheManager* cache)
{
    uint64_t total;

    if (cache == NULL || !cache->initialized) {
        return 0.0f;
    }

    total = cache->stats.total_hits + cache->stats.total_misses;
    if (total == 0) {
        return 0.0f;
    }

    return (float)cache->stats.total_hits / (float)total;
}

void Cache_ResetStats(CacheManager* cache)
{
    if (cache == NULL || !cache->initialized) {
        return;
    }

    cache->stats.total_hits = 0;
    cache->stats.total_misses = 0;
    cache->stats.total_inserts = 0;
    cache->stats.total_evictions = 0;
    cache->stats.total_removes = 0;
}

void Cache_PrintStats(CacheManager* cache)
{
    CacheStats stats;
    char size_buf[32], limit_buf[32];
    int i;

    if (cache == NULL || !cache->initialized) {
        printf("Cache: Not initialized\n");
        return;
    }

    Cache_GetStats(cache, &stats);
    Cache_FormatSize(stats.current_size, size_buf, sizeof(size_buf));
    Cache_FormatSize(stats.current_limit, limit_buf, sizeof(limit_buf));

    printf("=== Cache Statistics ===\n");
    printf("  Memory: %s / %s (%.1f%%)\n",
           size_buf, limit_buf, stats.fill_rate * 100.0f);
    printf("  Entries: %d (pinned: %d)\n",
           stats.entry_count, stats.pinned_count);
    printf("  Hit Rate: %.1f%% (hits=%llu, misses=%llu)\n",
           stats.hit_rate * 100.0f,
           (unsigned long long)stats.total_hits,
           (unsigned long long)stats.total_misses);
    printf("  Operations: inserts=%llu, evictions=%llu, removes=%llu\n",
           (unsigned long long)stats.total_inserts,
           (unsigned long long)stats.total_evictions,
           (unsigned long long)stats.total_removes);

    printf("  By Type:\n");
    for (i = 0; i < CACHE_TYPE_COUNT; i++) {
        if (stats.type_counts[i] > 0) {
            Cache_FormatSize(stats.type_sizes[i], size_buf, sizeof(size_buf));
            printf("    %s: %u entries, %s\n",
                   Cache_GetTypeName((CacheResourceType)i),
                   stats.type_counts[i], size_buf);
        }
    }
}

/* ========================================================================== */
/* Iteration API Implementation                                                */
/* ========================================================================== */

void Cache_Iterate(CacheManager* cache, CacheIterator callback, void* userdata)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || callback == NULL) {
        return;
    }

    entry = cache->lru_head;
    while (entry != NULL) {
        if (!callback(entry->key, entry->data, entry->size,
                      entry->ref_count, userdata)) {
            break;
        }
        entry = entry->lru_next;
    }
}

void Cache_IterateByType(CacheManager* cache, CacheResourceType type,
                         CacheIterator callback, void* userdata)
{
    CacheEntry* entry;

    if (cache == NULL || !cache->initialized || callback == NULL) {
        return;
    }

    entry = cache->lru_head;
    while (entry != NULL) {
        if (entry->type == type) {
            if (!callback(entry->key, entry->data, entry->size,
                          entry->ref_count, userdata)) {
                break;
            }
        }
        entry = entry->lru_next;
    }
}

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

const char* Cache_GetTypeName(CacheResourceType type)
{
    static const char* names[] = {
        "Texture",
        "Sprite",
        "Animation",
        "Sound",
        "Data",
        "Other"
    };

    if (type < 0 || type >= CACHE_TYPE_COUNT) {
        return "Unknown";
    }

    return names[type];
}

const char* Cache_FormatSize(size_t size, char* buffer, size_t buffer_size)
{
    static const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double s = (double)size;
    int unit = 0;

    if (buffer == NULL || buffer_size == 0) {
        return "";
    }

    while (s >= 1024.0 && unit < 4) {
        s /= 1024.0;
        unit++;
    }

    if (unit == 0) {
        snprintf(buffer, buffer_size, "%d %s", (int)size, units[0]);
    } else {
        snprintf(buffer, buffer_size, "%.1f %s", s, units[unit]);
    }

    return buffer;
}
