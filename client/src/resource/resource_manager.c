/**
 * @file resource_manager.c
 * @brief Unified Resource Manager Implementation
 *
 * Integrates:
 * - PVF library (libpvf.a) for Script.pvf
 * - NPK parser for ImagePacks
 * - IMG decoder for sprite data
 * - LRU cache for memory management
 *
 * @date 2026-01-04
 */

#include "resource/resource_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

/* Include PVF library */
#include "pvf/pvf.h"

/* ========================================================================== */
/* Internal Helpers                                                            */
/* ========================================================================== */

/* Find resource by path */
static ResourceHandle* find_resource(ResourceManager* mgr, const char* path)
{
    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            strcmp(mgr->resources[i].path, path) == 0) {
            return &mgr->resources[i];
        }
    }
    return NULL;
}

/* Allocate new resource slot */
static ResourceHandle* alloc_resource(ResourceManager* mgr)
{
    /* Find empty slot */
    for (int i = 0; i < mgr->resource_count; i++) {
        if (!mgr->resources[i].is_loaded && mgr->resources[i].ref_count == 0) {
            return &mgr->resources[i];
        }
    }

    /* Expand if room */
    if (mgr->resource_count < RESOURCE_MAX_CACHED) {
        return &mgr->resources[mgr->resource_count++];
    }

    /* Try to evict something */
    ResourceManager_EvictLRU(mgr, 1);

    /* Try again */
    for (int i = 0; i < mgr->resource_count; i++) {
        if (!mgr->resources[i].is_loaded && mgr->resources[i].ref_count == 0) {
            return &mgr->resources[i];
        }
    }

    return NULL;  /* No room */
}

/* Build path key for resource lookup */
static void build_resource_path(char* out, size_t out_size,
                                const char* npk_name, const char* img_path,
                                int frame_index)
{
    if (frame_index >= 0) {
        snprintf(out, out_size, "%s/%s#%d", npk_name, img_path, frame_index);
    } else {
        snprintf(out, out_size, "%s/%s", npk_name, img_path);
    }
}

/* Get or open NPK archive */
static NPKArchive* get_npk_archive(ResourceManager* mgr, const char* npk_name)
{
    /* Search already open archives */
    for (int i = 0; i < mgr->npk_count; i++) {
        if (mgr->npk_archives[i].is_open &&
            strcmp(mgr->npk_archives[i].name, npk_name) == 0) {
            return &mgr->npk_archives[i].archive;
        }
    }

    /* Open new archive */
    if (mgr->npk_count >= RESOURCE_MAX_NPK_ARCHIVES) {
        /* Too many archives - close oldest */
        for (int i = 0; i < mgr->npk_count; i++) {
            if (mgr->npk_archives[i].is_open) {
                NPK_Close(&mgr->npk_archives[i].archive);
                mgr->npk_archives[i].is_open = false;
                break;
            }
        }
    }

    /* Find empty slot */
    int slot = -1;
    for (int i = 0; i < RESOURCE_MAX_NPK_ARCHIVES; i++) {
        if (!mgr->npk_archives[i].is_open) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        return NULL;
    }

    /* Build full path */
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s",
             mgr->npk_directory, npk_name);

    /* Open NPK */
    NPKEntry* entry = &mgr->npk_archives[slot];
    if (NPK_Open(full_path, &entry->archive) != NPK_SUCCESS) {
        return NULL;
    }

    strncpy(entry->name, npk_name, sizeof(entry->name) - 1);
    entry->is_open = true;

    if (slot >= mgr->npk_count) {
        mgr->npk_count = slot + 1;
    }

    return &entry->archive;
}

/* Free resource data based on type */
static void free_resource_data(ResourceHandle* handle)
{
    if (handle->data == NULL) {
        return;
    }

    switch (handle->type) {
        case RESOURCE_TYPE_TEXTURE:
            SDL_DestroyTexture((SDL_Texture*)handle->data);
            break;

        case RESOURCE_TYPE_SPRITE:
            IMG_FreeSpriteSheet((IMGSpriteSheet*)handle->data);
            break;

        case RESOURCE_TYPE_ANIMATION:
            /* TODO: Animation_Free */
            free(handle->data);
            break;

        case RESOURCE_TYPE_DATA:
            free(handle->data);
            break;

        default:
            break;
    }

    handle->data = NULL;
    handle->is_loaded = false;
}

/* ========================================================================== */
/* LRU Cache Operations                                                        */
/* ========================================================================== */

/* Add resource to front of LRU list */
static void cache_add_front(ResourceManager* mgr, ResourceHandle* handle)
{
    CacheNode* node = (CacheNode*)malloc(sizeof(CacheNode));
    if (node == NULL) {
        return;
    }

    node->resource = handle;
    node->prev = NULL;
    node->next = mgr->cache_head;

    if (mgr->cache_head != NULL) {
        mgr->cache_head->prev = node;
    }
    mgr->cache_head = node;

    if (mgr->cache_tail == NULL) {
        mgr->cache_tail = node;
    }

    mgr->cache_size += handle->mem_size;
}

/* Remove resource from LRU list */
static void cache_remove(ResourceManager* mgr, ResourceHandle* handle)
{
    CacheNode* node = mgr->cache_head;
    while (node != NULL) {
        if (node->resource == handle) {
            if (node->prev != NULL) {
                node->prev->next = node->next;
            } else {
                mgr->cache_head = node->next;
            }

            if (node->next != NULL) {
                node->next->prev = node->prev;
            } else {
                mgr->cache_tail = node->prev;
            }

            mgr->cache_size -= handle->mem_size;
            free(node);
            return;
        }
        node = node->next;
    }
}

void ResourceManager_TouchCache(ResourceManager* mgr, ResourceHandle* handle)
{
    if (mgr == NULL || handle == NULL) {
        return;
    }

    /* Remove from current position */
    cache_remove(mgr, handle);

    /* Add to front */
    cache_add_front(mgr, handle);

    handle->last_access = SDL_GetTicks();
}

size_t ResourceManager_EvictLRU(ResourceManager* mgr, size_t required_size)
{
    if (mgr == NULL) {
        return 0;
    }

    size_t freed = 0;
    int evicted = 0;

    /* Evict from tail (least recently used) */
    while (mgr->cache_tail != NULL &&
           (freed < required_size ||
            mgr->cache_size + required_size > mgr->cache_limit)) {

        CacheNode* node = mgr->cache_tail;
        ResourceHandle* handle = node->resource;

        /* Only evict if not referenced */
        if (handle->ref_count == 0) {
            size_t size = handle->mem_size;
            cache_remove(mgr, handle);
            free_resource_data(handle);
            freed += size;
            evicted++;
            mgr->total_evictions++;
        } else {
            /* Move to front to avoid infinite loop */
            cache_remove(mgr, handle);
            cache_add_front(mgr, handle);
        }

        /* Safety limit */
        if (evicted > 100) {
            break;
        }
    }

    return freed;
}

/* ========================================================================== */
/* Lifecycle Implementation                                                    */
/* ========================================================================== */

int ResourceManager_Init(ResourceManager* mgr, SDL_Renderer* renderer,
                         const char* pvf_path, const char* npk_dir)
{
    if (mgr == NULL || renderer == NULL) {
        return RESOURCE_ERROR_INVALID;
    }

    memset(mgr, 0, sizeof(ResourceManager));
    mgr->renderer = renderer;
    mgr->cache_limit = RESOURCE_CACHE_DEFAULT_LIMIT;
    mgr->next_id = 1;

    /* Copy NPK directory */
    if (npk_dir != NULL) {
        strncpy(mgr->npk_directory, npk_dir, sizeof(mgr->npk_directory) - 1);
    }

    /* Initialize NPK subsystem */
    NPK_Initialize();

    /* Initialize PVF if path provided */
    if (pvf_path != NULL) {
        PVF_Initialize();

        mgr->pvf_pack = (PackSetInternal*)malloc(sizeof(PackSetInternal));
        if (mgr->pvf_pack != NULL) {
            if (PackSet_Open(pvf_path, mgr->pvf_pack) == PVF_SUCCESS) {
                mgr->pvf_loaded = true;
            } else {
                free(mgr->pvf_pack);
                mgr->pvf_pack = NULL;
            }
        }
    }

    return RESOURCE_SUCCESS;
}

void ResourceManager_Shutdown(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return;
    }

    /* Free all resources */
    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded) {
            free_resource_data(&mgr->resources[i]);
        }
    }

    /* Free LRU list */
    while (mgr->cache_head != NULL) {
        CacheNode* next = mgr->cache_head->next;
        free(mgr->cache_head);
        mgr->cache_head = next;
    }

    /* Close NPK archives */
    for (int i = 0; i < mgr->npk_count; i++) {
        if (mgr->npk_archives[i].is_open) {
            NPK_Close(&mgr->npk_archives[i].archive);
        }
    }

    /* Close PVF */
    if (mgr->pvf_pack != NULL) {
        PackSet_Close(mgr->pvf_pack);
        free(mgr->pvf_pack);
    }

    memset(mgr, 0, sizeof(ResourceManager));
}

bool ResourceManager_IsInitialized(ResourceManager* mgr)
{
    return mgr != NULL && mgr->renderer != NULL;
}

/* ========================================================================== */
/* Texture Loading Implementation                                              */
/* ========================================================================== */

SDL_Texture* ResourceManager_LoadTexture(ResourceManager* mgr,
                                         const char* npk_name,
                                         const char* img_path,
                                         int frame_index)
{
    if (!ResourceManager_IsInitialized(mgr) ||
        npk_name == NULL || img_path == NULL) {
        return NULL;
    }

    /* Build resource path */
    char path[256];
    build_resource_path(path, sizeof(path), npk_name, img_path, frame_index);

    /* Check cache */
    ResourceHandle* handle = find_resource(mgr, path);
    if (handle != NULL && handle->is_loaded) {
        handle->ref_count++;
        ResourceManager_TouchCache(mgr, handle);
        mgr->cache_hits++;
        return (SDL_Texture*)handle->data;
    }

    mgr->cache_misses++;
    mgr->total_loads++;

    /* Get NPK archive */
    NPKArchive* archive = get_npk_archive(mgr, npk_name);
    if (archive == NULL) {
        return NULL;
    }

    /* Find IMG file in NPK */
    NPKFileEntry* entry = NPK_FindFile(archive, img_path);
    if (entry == NULL) {
        return NULL;
    }

    /* Read IMG data */
    uint8_t* img_data = (uint8_t*)malloc(entry->size);
    if (img_data == NULL) {
        return NULL;
    }

    if (NPK_ReadFile(archive, entry, img_data, entry->size) != NPK_SUCCESS) {
        free(img_data);
        return NULL;
    }

    /* Parse IMG */
    IMGFile img;
    if (IMG_Parse(img_data, entry->size, &img) != IMG_SUCCESS) {
        free(img_data);
        return NULL;
    }

    /* Create texture from frame */
    SDL_Texture* texture = IMG_CreateTexture(mgr->renderer, &img, frame_index);

    /* Cleanup */
    IMG_Free(&img);
    free(img_data);

    if (texture == NULL) {
        return NULL;
    }

    /* Allocate resource handle */
    handle = alloc_resource(mgr);
    if (handle == NULL) {
        SDL_DestroyTexture(texture);
        return NULL;
    }

    /* Initialize handle */
    handle->id = mgr->next_id++;
    handle->type = RESOURCE_TYPE_TEXTURE;
    strncpy(handle->path, path, sizeof(handle->path) - 1);
    handle->data = texture;
    handle->ref_count = 1;
    handle->last_access = SDL_GetTicks();
    handle->is_loaded = true;

    /* Estimate memory size */
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    handle->mem_size = w * h * 4;  /* Assume RGBA */

    /* Add to cache */
    cache_add_front(mgr, handle);

    /* Evict if over limit */
    if (mgr->cache_size > mgr->cache_limit) {
        ResourceManager_EvictLRU(mgr, mgr->cache_size - mgr->cache_limit);
    }

    return texture;
}

void ResourceManager_ReleaseTexture(ResourceManager* mgr, SDL_Texture* texture)
{
    if (mgr == NULL || texture == NULL) {
        return;
    }

    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            mgr->resources[i].type == RESOURCE_TYPE_TEXTURE &&
            mgr->resources[i].data == texture) {
            if (mgr->resources[i].ref_count > 0) {
                mgr->resources[i].ref_count--;
            }
            return;
        }
    }
}

/* ========================================================================== */
/* Sprite Loading Implementation                                               */
/* ========================================================================== */

IMGSpriteSheet* ResourceManager_LoadSprite(ResourceManager* mgr,
                                           const char* npk_name,
                                           const char* img_path)
{
    if (!ResourceManager_IsInitialized(mgr) ||
        npk_name == NULL || img_path == NULL) {
        return NULL;
    }

    /* Build resource path */
    char path[256];
    build_resource_path(path, sizeof(path), npk_name, img_path, -1);

    /* Check cache */
    ResourceHandle* handle = find_resource(mgr, path);
    if (handle != NULL && handle->is_loaded) {
        handle->ref_count++;
        ResourceManager_TouchCache(mgr, handle);
        mgr->cache_hits++;
        return (IMGSpriteSheet*)handle->data;
    }

    mgr->cache_misses++;
    mgr->total_loads++;

    /* Get NPK archive */
    NPKArchive* archive = get_npk_archive(mgr, npk_name);
    if (archive == NULL) {
        return NULL;
    }

    /* Find IMG file */
    NPKFileEntry* entry = NPK_FindFile(archive, img_path);
    if (entry == NULL) {
        return NULL;
    }

    /* Read IMG data */
    uint8_t* img_data = (uint8_t*)malloc(entry->size);
    if (img_data == NULL) {
        return NULL;
    }

    if (NPK_ReadFile(archive, entry, img_data, entry->size) != NPK_SUCCESS) {
        free(img_data);
        return NULL;
    }

    /* Parse IMG */
    IMGFile img;
    if (IMG_Parse(img_data, entry->size, &img) != IMG_SUCCESS) {
        free(img_data);
        return NULL;
    }

    /* Create sprite sheet */
    IMGSpriteSheet* sheet = IMG_CreateSpriteSheet(mgr->renderer, &img);

    /* Cleanup */
    IMG_Free(&img);
    free(img_data);

    if (sheet == NULL) {
        return NULL;
    }

    /* Allocate handle */
    handle = alloc_resource(mgr);
    if (handle == NULL) {
        IMG_FreeSpriteSheet(sheet);
        return NULL;
    }

    handle->id = mgr->next_id++;
    handle->type = RESOURCE_TYPE_SPRITE;
    strncpy(handle->path, path, sizeof(handle->path) - 1);
    handle->data = sheet;
    handle->ref_count = 1;
    handle->last_access = SDL_GetTicks();
    handle->is_loaded = true;
    handle->mem_size = sheet->texture_width * sheet->texture_height * 4;

    cache_add_front(mgr, handle);

    return sheet;
}

void ResourceManager_ReleaseSprite(ResourceManager* mgr, IMGSpriteSheet* sprite)
{
    if (mgr == NULL || sprite == NULL) {
        return;
    }

    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            mgr->resources[i].type == RESOURCE_TYPE_SPRITE &&
            mgr->resources[i].data == sprite) {
            if (mgr->resources[i].ref_count > 0) {
                mgr->resources[i].ref_count--;
            }
            return;
        }
    }
}

/* ========================================================================== */
/* Animation Loading Implementation                                            */
/* ========================================================================== */

Animation* ResourceManager_LoadAnimation(ResourceManager* mgr,
                                         const char* ani_path)
{
    if (!ResourceManager_IsInitialized(mgr) ||
        !mgr->pvf_loaded || ani_path == NULL) {
        return NULL;
    }

    /* Check cache */
    ResourceHandle* handle = find_resource(mgr, ani_path);
    if (handle != NULL && handle->is_loaded) {
        handle->ref_count++;
        ResourceManager_TouchCache(mgr, handle);
        mgr->cache_hits++;
        return (Animation*)handle->data;
    }

    mgr->cache_misses++;

    /* TODO: Implement ANI loading from PVF */
    /* For now, return NULL - will be implemented in ani_loader.c */

    return NULL;
}

void ResourceManager_ReleaseAnimation(ResourceManager* mgr, Animation* anim)
{
    if (mgr == NULL || anim == NULL) {
        return;
    }

    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            mgr->resources[i].type == RESOURCE_TYPE_ANIMATION &&
            mgr->resources[i].data == anim) {
            if (mgr->resources[i].ref_count > 0) {
                mgr->resources[i].ref_count--;
            }
            return;
        }
    }
}

/* ========================================================================== */
/* Data Loading Implementation                                                 */
/* ========================================================================== */

int ResourceManager_LoadData(ResourceManager* mgr, const char* path,
                             void* buffer, size_t buffer_size,
                             size_t* actual_size)
{
    if (!ResourceManager_IsInitialized(mgr) ||
        !mgr->pvf_loaded || path == NULL || buffer == NULL) {
        return RESOURCE_ERROR_INVALID;
    }

    uint32_t size;
    int result = PackSet_ExtractFileByPath(mgr->pvf_pack, path,
                                           buffer, buffer_size, &size);
    if (result != PVF_SUCCESS) {
        return RESOURCE_ERROR_NOT_FOUND;
    }

    if (actual_size != NULL) {
        *actual_size = size;
    }

    return RESOURCE_SUCCESS;
}

/* ========================================================================== */
/* Cache Management Implementation                                             */
/* ========================================================================== */

void ResourceManager_SetCacheLimit(ResourceManager* mgr, size_t limit_bytes)
{
    if (mgr == NULL) {
        return;
    }
    mgr->cache_limit = limit_bytes;

    if (mgr->cache_size > mgr->cache_limit) {
        ResourceManager_EvictLRU(mgr, mgr->cache_size - mgr->cache_limit);
    }
}

size_t ResourceManager_GetCacheSize(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return 0;
    }
    return mgr->cache_size;
}

int ResourceManager_GarbageCollect(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return 0;
    }

    int collected = 0;
    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            mgr->resources[i].ref_count == 0) {
            cache_remove(mgr, &mgr->resources[i]);
            free_resource_data(&mgr->resources[i]);
            collected++;
        }
    }

    return collected;
}

void ResourceManager_ClearCache(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return;
    }

    for (int i = 0; i < mgr->resource_count; i++) {
        if (mgr->resources[i].is_loaded &&
            mgr->resources[i].ref_count == 0) {
            cache_remove(mgr, &mgr->resources[i]);
            free_resource_data(&mgr->resources[i]);
        }
    }
}

/* ========================================================================== */
/* Statistics Implementation                                                   */
/* ========================================================================== */

void ResourceManager_GetStats(ResourceManager* mgr,
                              uint32_t* hits, uint32_t* misses,
                              uint32_t* evictions)
{
    if (mgr == NULL) {
        return;
    }

    if (hits) *hits = mgr->cache_hits;
    if (misses) *misses = mgr->cache_misses;
    if (evictions) *evictions = mgr->total_evictions;
}

float ResourceManager_GetHitRate(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return 0.0f;
    }

    uint32_t total = mgr->cache_hits + mgr->cache_misses;
    if (total == 0) {
        return 0.0f;
    }

    return (float)mgr->cache_hits / (float)total;
}

void ResourceManager_PrintStats(ResourceManager* mgr)
{
    if (mgr == NULL) {
        printf("ResourceManager: (null)\n");
        return;
    }

    printf("ResourceManager Stats:\n");
    printf("  Cache size: %.2f MB / %.2f MB\n",
           mgr->cache_size / (1024.0f * 1024.0f),
           mgr->cache_limit / (1024.0f * 1024.0f));
    printf("  Resources: %d / %d\n", mgr->resource_count, RESOURCE_MAX_CACHED);
    printf("  NPK archives: %d\n", mgr->npk_count);
    printf("  PVF loaded: %s\n", mgr->pvf_loaded ? "yes" : "no");
    printf("  Cache hits: %u\n", mgr->cache_hits);
    printf("  Cache misses: %u\n", mgr->cache_misses);
    printf("  Hit rate: %.1f%%\n", ResourceManager_GetHitRate(mgr) * 100.0f);
    printf("  Total loads: %u\n", mgr->total_loads);
    printf("  Total evictions: %u\n", mgr->total_evictions);
}

/* ========================================================================== */
/* NPK Management Implementation                                               */
/* ========================================================================== */

int ResourceManager_PreloadNPK(ResourceManager* mgr, const char* npk_name)
{
    if (mgr == NULL || npk_name == NULL) {
        return RESOURCE_ERROR_INVALID;
    }

    NPKArchive* archive = get_npk_archive(mgr, npk_name);
    if (archive == NULL) {
        return RESOURCE_ERROR_LOAD_FAILED;
    }

    return RESOURCE_SUCCESS;
}

int ResourceManager_GetNPKCount(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < mgr->npk_count; i++) {
        if (mgr->npk_archives[i].is_open) {
            count++;
        }
    }
    return count;
}

void ResourceManager_CloseAllNPK(ResourceManager* mgr)
{
    if (mgr == NULL) {
        return;
    }

    for (int i = 0; i < mgr->npk_count; i++) {
        if (mgr->npk_archives[i].is_open) {
            NPK_Close(&mgr->npk_archives[i].archive);
            mgr->npk_archives[i].is_open = false;
        }
    }
    mgr->npk_count = 0;
}
