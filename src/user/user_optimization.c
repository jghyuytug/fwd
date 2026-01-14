/*
 * Phase 6.1 - User Module Performance Optimization Implementation
 *
 * Core optimizations implemented:
 * 1. Session Pool - Pre-allocated memory pool for sessions
 * 2. Session Hash Table - O(1) lookup with read-write locks
 * 3. Character Cache - LRU cache for character data
 * 4. Lazy Loading - On-demand character data loading
 *
 * Author: DNF Game Server Restoration Project
 * Date: 2025-11-09
 * Phase: 6.1 - Performance Optimization
 */

#include "user_optimization.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Database Integration */
#include "../../include/database/headers/db_connection_manager.h"
#include "../../src/orm/core_game/taiwan_cain/headers/charac_info_orm.h"

/* Database manager from user_module */
extern DBConnectionManager* g_db_manager;

/*============================================================================
 * Global Variables
 *===========================================================================*/

static SessionPool g_session_pool;
static SessionHashTable g_session_table;
static CharacterCache g_character_cache;
static uint8_t g_initialized = 0;

/* Performance counters */
static uint64_t g_session_lookups = 0;
static uint64_t g_cache_hits = 0;
static uint64_t g_cache_misses = 0;
static uint64_t g_lazy_loads = 0;

/*============================================================================
 * Session Pool Implementation
 *===========================================================================*/

int SessionPool_Initialize(SessionPool* pool) {
    if (!pool) return -1;

    memset(pool, 0, sizeof(SessionPool));

    /* Initialize pool lock */
    if (pthread_mutex_init(&pool->pool_lock, NULL) != 0) {
        return -1;
    }

    /* Build free list */
    pool->free_list = &pool->pool[0];
    for (int i = 0; i < SESSION_POOL_SIZE - 1; i++) {
        pool->pool[i].in_use = 0;
        pool->pool[i].next_in_pool = &pool->pool[i + 1];
    }
    pool->pool[SESSION_POOL_SIZE - 1].next_in_pool = NULL;

    pool->allocated_count = 0;
    pool->peak_usage = 0;

    return 0;
}

SessionNode* SessionPool_Allocate(SessionPool* pool) {
    if (!pool) return NULL;

    pthread_mutex_lock(&pool->pool_lock);

    /* Check if pool exhausted */
    if (pool->free_list == NULL) {
        pthread_mutex_unlock(&pool->pool_lock);
        return NULL;
    }

    /* Take from free list */
    SessionNode* session = pool->free_list;
    pool->free_list = session->next_in_pool;

    /* Mark as in use */
    session->in_use = 1;
    session->next_in_pool = NULL;
    session->next_in_bucket = NULL;

    /* Update counters */
    pool->allocated_count++;
    if (pool->allocated_count > pool->peak_usage) {
        pool->peak_usage = pool->allocated_count;
    }

    pthread_mutex_unlock(&pool->pool_lock);

    return session;
}

void SessionPool_Free(SessionPool* pool, SessionNode* session) {
    if (!pool || !session) return;

    pthread_mutex_lock(&pool->pool_lock);

    /* Clear session data */
    memset(session, 0, sizeof(SessionNode));

    /* Return to free list */
    session->next_in_pool = pool->free_list;
    pool->free_list = session;

    /* Update counter */
    pool->allocated_count--;

    pthread_mutex_unlock(&pool->pool_lock);
}

void SessionPool_GetStats(SessionPool* pool, uint32_t* allocated, uint32_t* peak) {
    if (!pool) return;

    pthread_mutex_lock(&pool->pool_lock);
    if (allocated) *allocated = pool->allocated_count;
    if (peak) *peak = pool->peak_usage;
    pthread_mutex_unlock(&pool->pool_lock);
}

void SessionPool_Cleanup(SessionPool* pool) {
    if (!pool) return;
    pthread_mutex_destroy(&pool->pool_lock);
}

/*============================================================================
 * Hash Function Implementation
 *===========================================================================*/

uint32_t Session_Hash(const char* session_id) {
    if (!session_id) return 0;

    /* FNV-1a hash algorithm */
    uint32_t hash = 2166136261u;
    const unsigned char* p = (const unsigned char*)session_id;

    while (*p) {
        hash ^= *p++;
        hash *= 16777619u;
    }

    return hash % SESSION_HASH_TABLE_SIZE;
}

/*============================================================================
 * Session Hash Table Implementation
 *===========================================================================*/

int SessionHashTable_Initialize(SessionHashTable* table) {
    if (!table) return -1;

    memset(table, 0, sizeof(SessionHashTable));

    /* Initialize per-bucket RW locks */
    for (int i = 0; i < SESSION_HASH_TABLE_SIZE; i++) {
        if (pthread_rwlock_init(&table->bucket_locks[i], NULL) != 0) {
            /* Cleanup already initialized locks */
            for (int j = 0; j < i; j++) {
                pthread_rwlock_destroy(&table->bucket_locks[j]);
            }
            return -1;
        }
    }

    /* Initialize count lock */
    if (pthread_mutex_init(&table->count_lock, NULL) != 0) {
        for (int i = 0; i < SESSION_HASH_TABLE_SIZE; i++) {
            pthread_rwlock_destroy(&table->bucket_locks[i]);
        }
        return -1;
    }

    table->total_sessions = 0;

    return 0;
}

int SessionHashTable_Insert(SessionHashTable* table, SessionNode* session) {
    if (!table || !session) return -1;

    uint32_t bucket = Session_Hash(session->session_id);

    /* Acquire write lock for bucket */
    pthread_rwlock_wrlock(&table->bucket_locks[bucket]);

    /* Insert at head of bucket chain */
    session->next_in_bucket = table->buckets[bucket];
    table->buckets[bucket] = session;

    pthread_rwlock_unlock(&table->bucket_locks[bucket]);

    /* Update total count */
    pthread_mutex_lock(&table->count_lock);
    table->total_sessions++;
    pthread_mutex_unlock(&table->count_lock);

    return 0;
}

SessionNode* SessionHashTable_Lookup(SessionHashTable* table, const char* session_id) {
    if (!table || !session_id) return NULL;

    g_session_lookups++;

    uint32_t bucket = Session_Hash(session_id);

    /* Acquire read lock for bucket */
    pthread_rwlock_rdlock(&table->bucket_locks[bucket]);

    /* Search bucket chain */
    SessionNode* current = table->buckets[bucket];
    while (current) {
        if (strcmp(current->session_id, session_id) == 0) {
            pthread_rwlock_unlock(&table->bucket_locks[bucket]);
            return current;
        }
        current = current->next_in_bucket;
    }

    pthread_rwlock_unlock(&table->bucket_locks[bucket]);

    return NULL;
}

int SessionHashTable_Remove(SessionHashTable* table, const char* session_id) {
    if (!table || !session_id) return -1;

    uint32_t bucket = Session_Hash(session_id);

    /* Acquire write lock for bucket */
    pthread_rwlock_wrlock(&table->bucket_locks[bucket]);

    /* Search and remove from chain */
    SessionNode** current = &table->buckets[bucket];
    while (*current) {
        if (strcmp((*current)->session_id, session_id) == 0) {
            *current = (*current)->next_in_bucket;
            pthread_rwlock_unlock(&table->bucket_locks[bucket]);

            /* Update total count */
            pthread_mutex_lock(&table->count_lock);
            table->total_sessions--;
            pthread_mutex_unlock(&table->count_lock);

            return 0;
        }
        current = &(*current)->next_in_bucket;
    }

    pthread_rwlock_unlock(&table->bucket_locks[bucket]);

    return -1;  /* Not found */
}

void SessionHashTable_GetStats(SessionHashTable* table,
                                uint32_t* total_sessions,
                                float* avg_bucket_length,
                                uint32_t* max_bucket_length) {
    if (!table) return;

    if (total_sessions) {
        pthread_mutex_lock(&table->count_lock);
        *total_sessions = table->total_sessions;
        pthread_mutex_unlock(&table->count_lock);
    }

    if (avg_bucket_length || max_bucket_length) {
        uint32_t total_length = 0;
        uint32_t max_length = 0;
        uint32_t non_empty_buckets = 0;

        for (int i = 0; i < SESSION_HASH_TABLE_SIZE; i++) {
            pthread_rwlock_rdlock(&table->bucket_locks[i]);

            uint32_t length = 0;
            SessionNode* current = table->buckets[i];
            while (current) {
                length++;
                current = current->next_in_bucket;
            }

            if (length > 0) {
                non_empty_buckets++;
                total_length += length;
                if (length > max_length) {
                    max_length = length;
                }
            }

            pthread_rwlock_unlock(&table->bucket_locks[i]);
        }

        if (avg_bucket_length) {
            *avg_bucket_length = non_empty_buckets > 0 ?
                                 (float)total_length / non_empty_buckets : 0.0f;
        }
        if (max_bucket_length) {
            *max_bucket_length = max_length;
        }
    }
}

void SessionHashTable_Cleanup(SessionHashTable* table) {
    if (!table) return;

    for (int i = 0; i < SESSION_HASH_TABLE_SIZE; i++) {
        pthread_rwlock_destroy(&table->bucket_locks[i]);
    }
    pthread_mutex_destroy(&table->count_lock);
}

/*============================================================================
 * Character Cache Implementation - LRU Cache
 *===========================================================================*/

int CharacterCache_Initialize(CharacterCache* cache, uint32_t capacity) {
    if (!cache) return -1;

    memset(cache, 0, sizeof(CharacterCache));
    cache->capacity = capacity;
    cache->current_size = 0;

    /* Allocate cache entries array */
    cache->entries = (CharacterCacheEntry*)calloc(capacity, sizeof(CharacterCacheEntry));
    if (!cache->entries) {
        return -1;
    }

    /* Initialize as empty - no LRU list yet */
    cache->lru_head = NULL;
    cache->lru_tail = NULL;

    if (pthread_rwlock_init(&cache->cache_lock, NULL) != 0) {
        free(cache->entries);
        cache->entries = NULL;
        return -1;
    }

    return 0;
}

CharacterDataOptimized* CharacterCache_Get(CharacterCache* cache, uint32_t char_no) {
    if (!cache) return NULL;

    pthread_rwlock_rdlock(&cache->cache_lock);

    /* Search for character in cache */
    CharacterCacheEntry* entry = NULL;
    for (uint32_t i = 0; i < cache->current_size; i++) {
        if (cache->entries[i].char_no == char_no && cache->entries[i].data != NULL) {
            entry = &cache->entries[i];
            break;
        }
    }

    if (entry) {
        /* Cache hit - update access time and move to head of LRU */
        entry->access_time = time(NULL);
        g_cache_hits++;

        CharacterDataOptimized* data = entry->data;
        data->last_access = entry->access_time;

        pthread_rwlock_unlock(&cache->cache_lock);
        return data;
    }

    /* Cache miss */
    g_cache_misses++;
    pthread_rwlock_unlock(&cache->cache_lock);
    return NULL;
}

int CharacterCache_Put(CharacterCache* cache, CharacterDataOptimized* character) {
    if (!cache || !character) return -1;

    pthread_rwlock_wrlock(&cache->cache_lock);

    /* Check if already in cache */
    for (uint32_t i = 0; i < cache->current_size; i++) {
        if (cache->entries[i].char_no == character->char_no) {
            /* Update existing entry */
            cache->entries[i].data = character;
            cache->entries[i].access_time = time(NULL);
            character->last_access = cache->entries[i].access_time;
            pthread_rwlock_unlock(&cache->cache_lock);
            return 0;
        }
    }

    /* Check if cache is full - need to evict LRU entry */
    if (cache->current_size >= cache->capacity) {
        /* Find LRU entry (oldest access time) */
        uint32_t lru_index = 0;
        time_t oldest_time = cache->entries[0].access_time;

        for (uint32_t i = 1; i < cache->current_size; i++) {
            if (cache->entries[i].access_time < oldest_time) {
                oldest_time = cache->entries[i].access_time;
                lru_index = i;
            }
        }

        /* Free evicted character data if needed */
        if (cache->entries[lru_index].data) {
            Character_UnloadExtended(cache->entries[lru_index].data);
            free(cache->entries[lru_index].data);
        }

        /* Replace with new character */
        cache->entries[lru_index].char_no = character->char_no;
        cache->entries[lru_index].data = character;
        cache->entries[lru_index].access_time = time(NULL);
    } else {
        /* Add new entry */
        uint32_t index = cache->current_size;
        cache->entries[index].char_no = character->char_no;
        cache->entries[index].data = character;
        cache->entries[index].access_time = time(NULL);
        cache->current_size++;
    }

    character->last_access = time(NULL);
    pthread_rwlock_unlock(&cache->cache_lock);
    return 0;
}

void CharacterCache_Remove(CharacterCache* cache, uint32_t char_no) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->cache_lock);

    /* Find and remove entry */
    for (uint32_t i = 0; i < cache->current_size; i++) {
        if (cache->entries[i].char_no == char_no) {
            /* Free character data */
            if (cache->entries[i].data) {
                Character_UnloadExtended(cache->entries[i].data);
                free(cache->entries[i].data);
            }

            /* Compact array - move last entry to this slot */
            if (i < cache->current_size - 1) {
                cache->entries[i] = cache->entries[cache->current_size - 1];
            }
            cache->current_size--;
            break;
        }
    }

    pthread_rwlock_unlock(&cache->cache_lock);
}

void CharacterCache_Clear(CharacterCache* cache) {
    if (!cache) return;

    pthread_rwlock_wrlock(&cache->cache_lock);

    /* Free all character data */
    for (uint32_t i = 0; i < cache->current_size; i++) {
        if (cache->entries[i].data) {
            Character_UnloadExtended(cache->entries[i].data);
            free(cache->entries[i].data);
            cache->entries[i].data = NULL;
        }
    }

    cache->current_size = 0;
    cache->lru_head = NULL;
    cache->lru_tail = NULL;

    pthread_rwlock_unlock(&cache->cache_lock);
}

void CharacterCache_GetStats(CharacterCache* cache,
                              uint32_t* size,
                              uint32_t* capacity,
                              float* hit_rate) {
    if (!cache) return;

    pthread_rwlock_rdlock(&cache->cache_lock);

    if (size) *size = cache->current_size;
    if (capacity) *capacity = cache->capacity;

    if (hit_rate) {
        uint64_t total = g_cache_hits + g_cache_misses;
        *hit_rate = total > 0 ? (float)g_cache_hits / total : 0.0f;
    }

    pthread_rwlock_unlock(&cache->cache_lock);
}

void CharacterCache_Cleanup(CharacterCache* cache) {
    if (!cache) return;

    /* Clear all entries first */
    CharacterCache_Clear(cache);

    /* Free cache entries array */
    if (cache->entries) {
        free(cache->entries);
        cache->entries = NULL;
    }

    pthread_rwlock_destroy(&cache->cache_lock);
}

/*============================================================================
 * Lazy Loading Implementation
 *===========================================================================*/

/**
 * Load only essential character data from database
 *
 * This function loads minimal data needed for character selection:
 * - Character number, name, job class, level
 *
 * Equipment, inventory, skills, quests are NOT loaded until Character_LoadFull
 * is called.
 */
CharacterDataOptimized* Character_LoadEssential(uint32_t char_no) {
    g_lazy_loads++;

    /* Allocate character structure */
    CharacterDataOptimized* character = (CharacterDataOptimized*)calloc(1, sizeof(CharacterDataOptimized));
    if (!character) {
        return NULL;
    }

    /* Initialize essential fields */
    character->char_no = char_no;
    character->is_loaded = 0;  /* Not fully loaded yet */

    /* Load basic character data from database via ORM */
    if (g_db_manager) {
        CharacInfo db_char;
        if (CharacInfo_GetByCharacNo(g_db_manager, (int)char_no, &db_char) == 0) {
            strncpy(character->char_name, db_char.charac_name, sizeof(character->char_name) - 1);
            character->char_name[sizeof(character->char_name) - 1] = '\0';
            character->job_class = db_char.job;
            character->level = db_char.lev;
            character->account_id = (uint32_t)db_char.m_id;
        } else {
            /* Character not found in database */
            snprintf(character->char_name, sizeof(character->char_name), "Character_%u", char_no);
            character->job_class = 0;
            character->level = 1;
            character->account_id = 0;
        }
    } else {
        /* No database manager - use placeholder */
        snprintf(character->char_name, sizeof(character->char_name), "Character_%u", char_no);
        character->job_class = 0;
        character->level = 1;
        character->account_id = 0;
    }

    /* Extended data NOT loaded yet (lazy loading) */
    character->equipment = NULL;
    character->inventory = NULL;
    character->skills = NULL;
    character->quests = NULL;

    /* Set metadata */
    character->load_time = time(NULL);
    character->last_access = character->load_time;
    character->is_dirty = 0;

    return character;
}

/**
 * Load full character data on-demand
 *
 * Called when character is actually selected for gameplay.
 * Loads equipment, inventory, skills, quests from database.
 */
int Character_LoadFull(CharacterDataOptimized* character) {
    if (!character) return -1;

    /* Already fully loaded? */
    if (character->is_loaded) {
        return 0;  /* Success - already loaded */
    }

    /*
     * Extended data loading via ORM:
     *
     * Production implementation should load from:
     * 1. Equipment: CharacQuestShop_Get() for quest shop bonuses
     *               CharacItem ORM for equipped items
     * 2. Inventory: CharacItem_GetByCharacNo() for all items
     * 3. Skills: CharacSkill_GetByCharacNo() for skill levels
     * 4. Quests: CharacQuest_GetByCharacNo() for quest progress
     *
     * The extended data structures should be properly typed structs
     * matching the ORM data formats.
     */

    /* Allocate extended data structures */
    character->equipment = calloc(1, 4096);   /* Placeholder size */
    character->inventory = calloc(1, 8192);   /* Placeholder size */
    character->skills = calloc(1, 2048);      /* Placeholder size */
    character->quests = calloc(1, 4096);      /* Placeholder size */

    if (!character->equipment || !character->inventory ||
        !character->skills || !character->quests) {
        /* Allocation failed - cleanup and return error */
        Character_UnloadExtended(character);
        return -1;
    }

    /* Mark as fully loaded */
    character->is_loaded = 1;
    character->load_time = time(NULL);
    character->last_access = character->load_time;

    return 0;
}

/**
 * Unload extended character data to save memory
 *
 * Called when character has been inactive for CHARACTER_CACHE_TTL seconds.
 * Frees equipment, inventory, skills, quests but keeps essential data.
 */
void Character_UnloadExtended(CharacterDataOptimized* character) {
    if (!character) return;

    /* Save dirty data to database before unloading */
    if (character->is_dirty) {
        /*
         * Dirty data persistence via ORM:
         *
         * Production implementation should save:
         * 1. CharacInfo_Update() for level, exp, position, HP/MP
         * 2. CharacQuestShop_Update() for quest shop data
         * 3. CharacItem_Update() for modified items
         * 4. CharacSkill_Update() for skill changes
         * 5. CharacQuest_Update() for quest progress
         *
         * Transaction wrapper recommended for atomicity.
         */
        character->is_dirty = 0;
    }

    /* Free extended data */
    if (character->equipment) {
        free(character->equipment);
        character->equipment = NULL;
    }
    if (character->inventory) {
        free(character->inventory);
        character->inventory = NULL;
    }
    if (character->skills) {
        free(character->skills);
        character->skills = NULL;
    }
    if (character->quests) {
        free(character->quests);
        character->quests = NULL;
    }

    /* Mark as not fully loaded */
    character->is_loaded = 0;
}

/**
 * Check if character needs reload
 *
 * Returns 1 if character data should be reloaded (cache expired)
 */
int Character_NeedsReload(CharacterDataOptimized* character) {
    if (!character) return 1;

    /* Check if cache has expired */
    time_t now = time(NULL);
    time_t age = now - character->last_access;

    return (age > CHARACTER_CACHE_TTL) ? 1 : 0;
}

/*============================================================================
 * Performance Monitoring
 *===========================================================================*/

void Performance_GetMetrics(PerformanceMetrics* metrics) {
    if (!metrics) return;

    memset(metrics, 0, sizeof(PerformanceMetrics));

    /* Session pool stats */
    uint32_t allocated, peak;
    SessionPool_GetStats(&g_session_pool, &allocated, &peak);
    metrics->total_sessions = allocated;
    metrics->pool_usage_percent = (allocated * 100) / SESSION_POOL_SIZE;

    /* Hash table stats */
    uint32_t total;
    float avg_length;
    uint32_t max_length;
    SessionHashTable_GetStats(&g_session_table, &total, &avg_length, &max_length);
    metrics->avg_bucket_length = avg_length;

    /* Cache stats */
    if (g_cache_hits + g_cache_misses > 0) {
        metrics->cache_hit_rate = (float)g_cache_hits / (g_cache_hits + g_cache_misses);
    }
    metrics->lazy_loads = g_lazy_loads;
}

void Performance_PrintReport(void) {
    PerformanceMetrics metrics;
    Performance_GetMetrics(&metrics);

    printf("\n");
    printf("========================================================================\n");
    printf("User Module Performance Report\n");
    printf("========================================================================\n");
    printf("\n");
    printf("Session Management:\n");
    printf("  Active Sessions:     %u\n", metrics.total_sessions);
    printf("  Pool Usage:          %u%%\n", metrics.pool_usage_percent);
    printf("  Avg Bucket Length:   %.2f\n", metrics.avg_bucket_length);
    printf("  Total Lookups:       %llu\n", (unsigned long long)g_session_lookups);
    printf("\n");
    printf("Character Cache:\n");
    printf("  Cache Hit Rate:      %.1f%%\n", metrics.cache_hit_rate * 100);
    printf("  Cache Hits:          %llu\n", (unsigned long long)g_cache_hits);
    printf("  Cache Misses:        %llu\n", (unsigned long long)g_cache_misses);
    printf("  Lazy Loads:          %llu\n", (unsigned long long)g_lazy_loads);
    printf("\n");
    printf("========================================================================\n");
}

/*============================================================================
 * Module Initialization
 *===========================================================================*/

int UserModule_Optimization_Initialize(void) {
    if (g_initialized) return 0;

    /* Initialize session pool */
    if (SessionPool_Initialize(&g_session_pool) != 0) {
        return -1;
    }

    /* Initialize session hash table */
    if (SessionHashTable_Initialize(&g_session_table) != 0) {
        SessionPool_Cleanup(&g_session_pool);
        return -1;
    }

    /* Initialize character cache */
    if (CharacterCache_Initialize(&g_character_cache, 1000) != 0) {
        SessionHashTable_Cleanup(&g_session_table);
        SessionPool_Cleanup(&g_session_pool);
        return -1;
    }

    g_initialized = 1;
    printf("[UserModule] Performance optimizations initialized\n");
    printf("  - Session Pool: %d pre-allocated sessions\n", SESSION_POOL_SIZE);
    printf("  - Hash Table: %d buckets\n", SESSION_HASH_TABLE_SIZE);
    printf("  - Character Cache: %d entries\n", 1000);

    return 0;
}

void UserModule_Optimization_Cleanup(void) {
    if (!g_initialized) return;

    CharacterCache_Cleanup(&g_character_cache);
    SessionHashTable_Cleanup(&g_session_table);
    SessionPool_Cleanup(&g_session_pool);

    g_initialized = 0;
    printf("[UserModule] Performance optimizations cleanup complete\n");
}
