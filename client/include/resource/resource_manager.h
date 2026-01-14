/**
 * @file resource_manager.h
 * @brief Unified Resource Manager for DNF Client
 *
 * Integrates PVF and NPK resource loading with:
 * - Unified API for textures, animations, and data
 * - LRU cache with configurable size limit
 * - Reference counting for resource lifetime
 * - Lazy NPK loading (on-demand)
 *
 * @date 2026-01-04
 */

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "resource/npk_parser.h"
#include "resource/img_decoder.h"

/* Forward declarations */
struct SDL_Renderer;
struct SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

/* Forward from PVF library */
struct PackSetInternal;
typedef struct PackSetInternal PackSetInternal;

/* Forward from client */
struct Animation;
typedef struct Animation Animation;

/* ========================================================================== */
/* Constants                                                                   */
/* ========================================================================== */

/* Default cache limit: 256MB */
#define RESOURCE_CACHE_DEFAULT_LIMIT    (256 * 1024 * 1024)

/* Maximum NPK archives open at once */
#define RESOURCE_MAX_NPK_ARCHIVES       64

/* Maximum cached resources */
#define RESOURCE_MAX_CACHED             4096

/* Resource types */
typedef enum ResourceType {
    RESOURCE_TYPE_TEXTURE = 0,  /* SDL_Texture from IMG */
    RESOURCE_TYPE_ANIMATION,    /* Animation from ANI */
    RESOURCE_TYPE_SPRITE,       /* IMGSpriteSheet */
    RESOURCE_TYPE_DATA,         /* Raw data from PVF */
    RESOURCE_TYPE_COUNT
} ResourceType;

/* Return codes */
#define RESOURCE_SUCCESS            0
#define RESOURCE_ERROR_NOT_INIT    -1
#define RESOURCE_ERROR_NOT_FOUND   -2
#define RESOURCE_ERROR_MEMORY      -3
#define RESOURCE_ERROR_LOAD_FAILED -4
#define RESOURCE_ERROR_INVALID     -5

/* ========================================================================== */
/* Data Structures                                                             */
/* ========================================================================== */

/**
 * @brief Cached resource handle
 */
typedef struct ResourceHandle {
    uint32_t        id;             /* Unique resource ID */
    ResourceType    type;           /* Resource type */
    char            path[256];      /* Resource path (for lookup) */
    void*           data;           /* Actual data pointer */
    uint32_t        ref_count;      /* Reference count */
    uint32_t        last_access;    /* Last access time (ms) */
    size_t          mem_size;       /* Estimated memory size */
    bool            is_loaded;      /* True if loaded */
} ResourceHandle;

/**
 * @brief LRU cache node
 */
typedef struct CacheNode {
    ResourceHandle*     resource;
    struct CacheNode*   prev;
    struct CacheNode*   next;
} CacheNode;

/**
 * @brief NPK archive entry
 */
typedef struct NPKEntry {
    char            name[64];       /* NPK filename (without path) */
    NPKArchive      archive;        /* NPK handle */
    bool            is_open;        /* True if open */
} NPKEntry;

/**
 * @brief Resource Manager
 */
typedef struct ResourceManager {
    /* SDL Renderer */
    SDL_Renderer*   renderer;

    /* PVF Manager */
    PackSetInternal* pvf_pack;      /* Script.pvf handle */
    bool            pvf_loaded;

    /* NPK Archives */
    NPKEntry        npk_archives[RESOURCE_MAX_NPK_ARCHIVES];
    int             npk_count;
    char            npk_directory[256]; /* Base NPK directory */

    /* Resource Pool */
    ResourceHandle  resources[RESOURCE_MAX_CACHED];
    int             resource_count;
    uint32_t        next_id;

    /* LRU Cache */
    CacheNode*      cache_head;     /* Most recently used */
    CacheNode*      cache_tail;     /* Least recently used */
    size_t          cache_size;     /* Current cache size (bytes) */
    size_t          cache_limit;    /* Maximum cache size */

    /* Statistics */
    uint32_t        cache_hits;
    uint32_t        cache_misses;
    uint32_t        total_loads;
    uint32_t        total_evictions;
} ResourceManager;

/* ========================================================================== */
/* Lifecycle API                                                               */
/* ========================================================================== */

/**
 * @brief Initialize resource manager
 *
 * @param mgr           Resource manager to initialize
 * @param renderer      SDL renderer
 * @param pvf_path      Path to Script.pvf (or NULL to skip PVF)
 * @param npk_dir       Path to NPK directory (ImagePacks2/)
 *
 * @return RESOURCE_SUCCESS on success
 */
int ResourceManager_Init(ResourceManager* mgr, SDL_Renderer* renderer,
                         const char* pvf_path, const char* npk_dir);

/**
 * @brief Shutdown resource manager
 *
 * @param mgr           Resource manager to shutdown
 */
void ResourceManager_Shutdown(ResourceManager* mgr);

/**
 * @brief Check if resource manager is initialized
 *
 * @param mgr           Resource manager
 * @return true if initialized
 */
bool ResourceManager_IsInitialized(ResourceManager* mgr);

/* ========================================================================== */
/* Texture Loading API                                                         */
/* ========================================================================== */

/**
 * @brief Load texture from NPK/IMG
 *
 * @param mgr           Resource manager
 * @param npk_name      NPK filename (e.g., "sprite_character.npk")
 * @param img_path      IMG path within NPK (e.g., "character/swordman.img")
 * @param frame_index   Frame index (0-based)
 *
 * @return SDL_Texture* or NULL on error
 *
 * @note Returned texture is managed by resource manager.
 *       Call ResourceManager_ReleaseTexture when done.
 */
SDL_Texture* ResourceManager_LoadTexture(ResourceManager* mgr,
                                         const char* npk_name,
                                         const char* img_path,
                                         int frame_index);

/**
 * @brief Release texture reference
 *
 * @param mgr           Resource manager
 * @param texture       Texture to release
 */
void ResourceManager_ReleaseTexture(ResourceManager* mgr, SDL_Texture* texture);

/* ========================================================================== */
/* Sprite Loading API                                                          */
/* ========================================================================== */

/**
 * @brief Load sprite sheet from NPK/IMG
 *
 * @param mgr           Resource manager
 * @param npk_name      NPK filename
 * @param img_path      IMG path within NPK
 *
 * @return IMGSpriteSheet* or NULL on error
 *
 * @note Returned sprite sheet is managed by resource manager.
 */
IMGSpriteSheet* ResourceManager_LoadSprite(ResourceManager* mgr,
                                           const char* npk_name,
                                           const char* img_path);

/**
 * @brief Release sprite sheet reference
 *
 * @param mgr           Resource manager
 * @param sprite        Sprite sheet to release
 */
void ResourceManager_ReleaseSprite(ResourceManager* mgr, IMGSpriteSheet* sprite);

/* ========================================================================== */
/* Animation Loading API                                                       */
/* ========================================================================== */

/**
 * @brief Load animation from PVF
 *
 * @param mgr           Resource manager
 * @param ani_path      ANI file path in PVF (e.g., "creature/xxx.ani")
 *
 * @return Animation* or NULL on error
 */
Animation* ResourceManager_LoadAnimation(ResourceManager* mgr,
                                         const char* ani_path);

/**
 * @brief Release animation reference
 *
 * @param mgr           Resource manager
 * @param anim          Animation to release
 */
void ResourceManager_ReleaseAnimation(ResourceManager* mgr, Animation* anim);

/* ========================================================================== */
/* Data Loading API                                                            */
/* ========================================================================== */

/**
 * @brief Load raw data from PVF
 *
 * @param mgr           Resource manager
 * @param path          File path in PVF
 * @param buffer        Output buffer (caller allocates)
 * @param buffer_size   Buffer size
 * @param actual_size   Output: actual size read
 *
 * @return RESOURCE_SUCCESS on success
 */
int ResourceManager_LoadData(ResourceManager* mgr, const char* path,
                             void* buffer, size_t buffer_size,
                             size_t* actual_size);

/* ========================================================================== */
/* Cache Management API                                                        */
/* ========================================================================== */

/**
 * @brief Set cache size limit
 *
 * @param mgr           Resource manager
 * @param limit_bytes   New limit in bytes
 */
void ResourceManager_SetCacheLimit(ResourceManager* mgr, size_t limit_bytes);

/**
 * @brief Get current cache size
 *
 * @param mgr           Resource manager
 * @return Current cache size in bytes
 */
size_t ResourceManager_GetCacheSize(ResourceManager* mgr);

/**
 * @brief Force garbage collection
 *
 * Evicts resources with ref_count == 0 until cache is under limit.
 *
 * @param mgr           Resource manager
 * @return Number of resources evicted
 */
int ResourceManager_GarbageCollect(ResourceManager* mgr);

/**
 * @brief Clear all cached resources
 *
 * @param mgr           Resource manager
 *
 * @warning Resources with ref_count > 0 will NOT be cleared.
 */
void ResourceManager_ClearCache(ResourceManager* mgr);

/* ========================================================================== */
/* Statistics API                                                              */
/* ========================================================================== */

/**
 * @brief Get cache statistics
 *
 * @param mgr           Resource manager
 * @param hits          Output: cache hits
 * @param misses        Output: cache misses
 * @param evictions     Output: total evictions
 */
void ResourceManager_GetStats(ResourceManager* mgr,
                              uint32_t* hits, uint32_t* misses,
                              uint32_t* evictions);

/**
 * @brief Get cache hit rate
 *
 * @param mgr           Resource manager
 * @return Hit rate (0.0 - 1.0)
 */
float ResourceManager_GetHitRate(ResourceManager* mgr);

/**
 * @brief Print cache stats (for debugging)
 *
 * @param mgr           Resource manager
 */
void ResourceManager_PrintStats(ResourceManager* mgr);

/* ========================================================================== */
/* NPK Management API                                                          */
/* ========================================================================== */

/**
 * @brief Preload NPK archive
 *
 * @param mgr           Resource manager
 * @param npk_name      NPK filename to preload
 *
 * @return RESOURCE_SUCCESS on success
 */
int ResourceManager_PreloadNPK(ResourceManager* mgr, const char* npk_name);

/**
 * @brief Get number of loaded NPK archives
 *
 * @param mgr           Resource manager
 * @return Number of open NPK archives
 */
int ResourceManager_GetNPKCount(ResourceManager* mgr);

/**
 * @brief Close all NPK archives (free memory)
 *
 * @param mgr           Resource manager
 */
void ResourceManager_CloseAllNPK(ResourceManager* mgr);

/* ========================================================================== */
/* Internal API (for cache_manager.c)                                          */
/* ========================================================================== */

/**
 * @brief Touch resource (move to front of LRU)
 *
 * @param mgr           Resource manager
 * @param handle        Resource to touch
 */
void ResourceManager_TouchCache(ResourceManager* mgr, ResourceHandle* handle);

/**
 * @brief Evict least recently used resource
 *
 * @param mgr           Resource manager
 * @param required_size Size needed (bytes)
 *
 * @return Number of bytes freed
 */
size_t ResourceManager_EvictLRU(ResourceManager* mgr, size_t required_size);

#ifdef __cplusplus
}
#endif

#endif /* RESOURCE_MANAGER_H */
