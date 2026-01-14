/*
 * Phase 6.1 - User Module Performance Optimization Header
 *
 * This file contains optimized data structures and function declarations
 * for memory and concurrency improvements.
 *
 * Optimizations:
 * 1. Lazy Loading - Load character data on-demand
 * 2. Session Pool - Reuse session objects
 * 3. Read-Write Locks - Separate read/write access
 * 4. Hash Table - O(1) session lookup
 *
 * Author: DNF Game Server Restoration Project
 * Date: 2025-11-09
 * Phase: 6.1 - Performance Optimization
 */

#ifndef USER_MODULE_OPTIMIZATION_H
#define USER_MODULE_OPTIMIZATION_H

/* Enable POSIX features for pthread_rwlock_t */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>
#include <stdint.h>
#include <time.h>

/*============================================================================
 * Configuration Constants
 *===========================================================================*/

#define SESSION_POOL_SIZE           1024    /* Pre-allocated session pool */
#define SESSION_HASH_TABLE_SIZE     8192    /* Hash table buckets (2^13) */
#define MAX_CHARACTERS_PER_ACCOUNT  8       /* Maximum characters per account */
#define CHARACTER_CACHE_TTL         300     /* Character cache TTL (seconds) */
#define SESSION_CLEANUP_INTERVAL    60      /* Session cleanup interval (seconds) */

/*============================================================================
 * Optimized Data Structures
 *===========================================================================*/

/**
 * Character Data - Lazy Loading Support
 *
 * Only essential fields are loaded initially.
 * Full character data is loaded on-demand when accessed.
 */
typedef struct {
    /* Essential fields (always loaded) */
    uint32_t char_no;                   /* Character number */
    uint32_t account_id;                /* Owner account ID */
    char char_name[32];                 /* Character name */
    uint8_t job_class;                  /* Job/class type */
    uint16_t level;                     /* Character level */
    uint8_t is_loaded;                  /* Full data loaded flag */

    /* Extended fields (lazy loaded) */
    void* equipment;                    /* Equipment data (lazy) */
    void* inventory;                    /* Inventory data (lazy) */
    void* skills;                       /* Skill data (lazy) */
    void* quests;                       /* Quest data (lazy) */

    /* Cache metadata */
    time_t load_time;                   /* When full data was loaded */
    time_t last_access;                 /* Last access timestamp */
    uint8_t is_dirty;                   /* Needs database sync */
} CharacterDataOptimized;

/**
 * Account Data - Memory Optimized
 *
 * Reduced memory footprint by:
 * - Using fixed-size arrays instead of pointers
 * - Lazy loading character data
 * - Compact bit fields
 */
typedef struct {
    /* Core account info */
    uint32_t account_id;                /* Account ID */
    char account_name[32];              /* Username */
    uint8_t account_status;             /* Account status */
    uint8_t gm_level;                   /* GM privilege level */

    /* Character list (lazy loaded) */
    uint8_t character_count;            /* Number of characters */
    uint32_t character_ids[MAX_CHARACTERS_PER_ACCOUNT];  /* Character IDs only */
    CharacterDataOptimized* characters[MAX_CHARACTERS_PER_ACCOUNT];  /* Lazy loaded */

    /* Session info */
    uint32_t active_char_no;            /* Currently active character */
    time_t last_activity;               /* Last activity timestamp */

    /* Flags (using bit fields to save memory) */
    uint8_t is_online : 1;              /* Online status */
    uint8_t has_loaded_chars : 1;       /* Characters loaded flag */
    uint8_t reserved : 6;               /* Reserved bits */
} AccountDataOptimized;

/**
 * Session Object - Pool Allocated
 *
 * Sessions are pre-allocated in a pool and reused
 * to avoid frequent malloc/free.
 */
typedef struct SessionNode {
    /* Session data */
    char session_id[64];                /* Session identifier */
    uint32_t account_id;                /* Associated account */
    AccountDataOptimized* account;      /* Account data pointer */

    /* Network info */
    uint32_t client_ip;                 /* Client IP address */
    uint16_t client_port;               /* Client port */

    /* Timestamps */
    time_t create_time;                 /* Session creation time */
    time_t last_heartbeat;              /* Last heartbeat time */

    /* Pool management */
    uint8_t in_use;                     /* Session in use flag */
    struct SessionNode* next_in_pool;   /* Next free session in pool */
    struct SessionNode* next_in_bucket; /* Next session in hash bucket */
} SessionNode;

/**
 * Session Pool - Memory Pool for Sessions
 */
typedef struct {
    SessionNode pool[SESSION_POOL_SIZE]; /* Pre-allocated sessions */
    SessionNode* free_list;              /* Free sessions list */
    uint32_t allocated_count;            /* Number of allocated sessions */
    uint32_t peak_usage;                 /* Peak usage tracking */
    pthread_mutex_t pool_lock;           /* Pool allocation lock */
} SessionPool;

/**
 * Session Hash Table - O(1) Lookup
 */
typedef struct {
    SessionNode* buckets[SESSION_HASH_TABLE_SIZE];  /* Hash buckets */
    pthread_rwlock_t bucket_locks[SESSION_HASH_TABLE_SIZE];  /* Per-bucket RW locks */
    uint32_t total_sessions;             /* Total active sessions */
    pthread_mutex_t count_lock;          /* Count update lock */
} SessionHashTable;

/**
 * Character Cache Entry - LRU Cache
 */
typedef struct CacheEntry {
    uint32_t char_no;                    /* Character number (key) */
    CharacterDataOptimized* data;        /* Cached character data */
    time_t access_time;                  /* Last access time */
    struct CacheEntry* prev;             /* LRU list - previous */
    struct CacheEntry* next;             /* LRU list - next */
} CharacterCacheEntry;

/**
 * Character Cache - LRU Cache for Character Data
 */
typedef struct {
    CharacterCacheEntry* entries;        /* Cache entries */
    CharacterCacheEntry* lru_head;       /* LRU list head (most recent) */
    CharacterCacheEntry* lru_tail;       /* LRU list tail (least recent) */
    uint32_t capacity;                   /* Maximum cache size */
    uint32_t current_size;               /* Current cache size */
    pthread_rwlock_t cache_lock;         /* Cache access lock */
} CharacterCache;

/*============================================================================
 * Function Declarations - Session Pool
 *===========================================================================*/

/**
 * Initialize session pool
 *
 * Pre-allocates SESSION_POOL_SIZE sessions for reuse.
 *
 * @param pool Pointer to session pool structure
 * @return 0 on success, negative on error
 */
int SessionPool_Initialize(SessionPool* pool);

/**
 * Allocate a session from pool
 *
 * @param pool Session pool
 * @return Pointer to allocated session, NULL if pool exhausted
 */
SessionNode* SessionPool_Allocate(SessionPool* pool);

/**
 * Return a session to pool
 *
 * @param pool Session pool
 * @param session Session to return
 */
void SessionPool_Free(SessionPool* pool, SessionNode* session);

/**
 * Get pool statistics
 *
 * @param pool Session pool
 * @param allocated Output: number of allocated sessions
 * @param peak Output: peak usage
 */
void SessionPool_GetStats(SessionPool* pool, uint32_t* allocated, uint32_t* peak);

/**
 * Cleanup session pool
 *
 * @param pool Session pool
 */
void SessionPool_Cleanup(SessionPool* pool);

/*============================================================================
 * Function Declarations - Session Hash Table
 *===========================================================================*/

/**
 * Initialize session hash table
 *
 * @param table Hash table structure
 * @return 0 on success, negative on error
 */
int SessionHashTable_Initialize(SessionHashTable* table);

/**
 * Insert session into hash table
 *
 * Uses read-write lock for bucket-level concurrency.
 *
 * @param table Hash table
 * @param session Session to insert
 * @return 0 on success, negative on error
 */
int SessionHashTable_Insert(SessionHashTable* table, SessionNode* session);

/**
 * Lookup session by session_id
 *
 * Uses read lock for high-concurrency reads.
 *
 * @param table Hash table
 * @param session_id Session ID to lookup
 * @return Pointer to session, NULL if not found
 */
SessionNode* SessionHashTable_Lookup(SessionHashTable* table, const char* session_id);

/**
 * Remove session from hash table
 *
 * @param table Hash table
 * @param session_id Session ID to remove
 * @return 0 on success, negative if not found
 */
int SessionHashTable_Remove(SessionHashTable* table, const char* session_id);

/**
 * Get hash table statistics
 *
 * @param table Hash table
 * @param total_sessions Output: total active sessions
 * @param avg_bucket_length Output: average bucket chain length
 * @param max_bucket_length Output: maximum bucket chain length
 */
void SessionHashTable_GetStats(SessionHashTable* table,
                                uint32_t* total_sessions,
                                float* avg_bucket_length,
                                uint32_t* max_bucket_length);

/**
 * Cleanup hash table
 *
 * @param table Hash table
 */
void SessionHashTable_Cleanup(SessionHashTable* table);

/*============================================================================
 * Function Declarations - Lazy Loading
 *===========================================================================*/

/**
 * Load character essential data only
 *
 * Loads only char_no, name, job, level for fast character list display.
 *
 * @param char_no Character number
 * @return Pointer to character data (essential only), NULL on error
 */
CharacterDataOptimized* Character_LoadEssential(uint32_t char_no);

/**
 * Load full character data on-demand
 *
 * Loads equipment, inventory, skills, quests when character is selected.
 *
 * @param character Character with essential data already loaded
 * @return 0 on success, negative on error
 */
int Character_LoadFull(CharacterDataOptimized* character);

/**
 * Unload character extended data
 *
 * Frees memory for equipment/inventory/skills/quests.
 * Call when character is inactive for CHARACTER_CACHE_TTL seconds.
 *
 * @param character Character data
 */
void Character_UnloadExtended(CharacterDataOptimized* character);

/**
 * Check if character needs reload
 *
 * @param character Character data
 * @return 1 if reload needed, 0 otherwise
 */
int Character_NeedsReload(CharacterDataOptimized* character);

/*============================================================================
 * Function Declarations - Character Cache
 *===========================================================================*/

/**
 * Initialize character cache
 *
 * @param cache Cache structure
 * @param capacity Maximum number of cached characters
 * @return 0 on success, negative on error
 */
int CharacterCache_Initialize(CharacterCache* cache, uint32_t capacity);

/**
 * Get character from cache
 *
 * @param cache Character cache
 * @param char_no Character number
 * @return Cached character data, NULL if not in cache
 */
CharacterDataOptimized* CharacterCache_Get(CharacterCache* cache, uint32_t char_no);

/**
 * Put character into cache
 *
 * Uses LRU eviction if cache is full.
 *
 * @param cache Character cache
 * @param character Character data to cache
 * @return 0 on success, negative on error
 */
int CharacterCache_Put(CharacterCache* cache, CharacterDataOptimized* character);

/**
 * Remove character from cache
 *
 * @param cache Character cache
 * @param char_no Character number
 */
void CharacterCache_Remove(CharacterCache* cache, uint32_t char_no);

/**
 * Clear entire cache
 *
 * @param cache Character cache
 */
void CharacterCache_Clear(CharacterCache* cache);

/**
 * Get cache statistics
 *
 * @param cache Character cache
 * @param size Output: current cache size
 * @param capacity Output: cache capacity
 * @param hit_rate Output: cache hit rate (0.0-1.0)
 */
void CharacterCache_GetStats(CharacterCache* cache,
                              uint32_t* size,
                              uint32_t* capacity,
                              float* hit_rate);

/**
 * Cleanup character cache
 *
 * @param cache Character cache
 */
void CharacterCache_Cleanup(CharacterCache* cache);

/*============================================================================
 * Function Declarations - Concurrency Optimization
 *===========================================================================*/

/**
 * Hash function for session_id
 *
 * Uses FNV-1a hash algorithm for good distribution.
 *
 * @param session_id Session ID string
 * @return Hash value (0 to SESSION_HASH_TABLE_SIZE-1)
 */
uint32_t Session_Hash(const char* session_id);

/**
 * Periodic session cleanup task
 *
 * Removes expired sessions, compacts hash table.
 * Should be called every SESSION_CLEANUP_INTERVAL seconds.
 *
 * @param table Session hash table
 * @param pool Session pool
 * @return Number of sessions cleaned up
 */
uint32_t Session_PeriodicCleanup(SessionHashTable* table, SessionPool* pool);

/*============================================================================
 * Performance Monitoring
 *===========================================================================*/

/**
 * Performance metrics structure
 */
typedef struct {
    /* Session metrics */
    uint32_t total_sessions;             /* Total active sessions */
    uint32_t pool_usage_percent;         /* Pool usage percentage */
    uint32_t hash_collisions;            /* Hash collision count */
    float avg_bucket_length;             /* Average bucket length */

    /* Character cache metrics */
    uint32_t cache_size;                 /* Current cache size */
    float cache_hit_rate;                /* Cache hit rate */
    uint32_t lazy_loads;                 /* Lazy load count */

    /* Timing metrics */
    uint64_t avg_session_lookup_us;      /* Avg session lookup time (μs) */
    uint64_t avg_char_load_us;           /* Avg character load time (μs) */
    uint64_t avg_char_switch_us;         /* Avg character switch time (μs) */
} PerformanceMetrics;

/**
 * Get current performance metrics
 *
 * @param metrics Output: performance metrics structure
 */
void Performance_GetMetrics(PerformanceMetrics* metrics);

/**
 * Print performance report
 *
 * Outputs detailed performance statistics to stdout.
 */
void Performance_PrintReport(void);

#endif /* USER_MODULE_OPTIMIZATION_H */
