/**
 * @file pvf_manager.c
 * @brief PVF数据管理器实现
 */

#include "pvf_manager.h"
#include "../../include/pvf/pvf.h"
#include "../../include/pvf/pvf_packset.h"
#include "../../include/pvf/pvf_stringtable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* 内部状态                                                                    */
/* ========================================================================== */

static struct {
    int initialized;
    PackSetInternal pack;           /* PackSet实例（非指针） */
    PVFStringTable stringtable;     /* StringTable实例 */
    char pvf_path[512];
    char stringtable_path[512];
    int stringtable_loaded;
} g_pvf_state = {0};

/* ========================================================================== */
/* 初始化/清理                                                                 */
/* ========================================================================== */

int PVF_Manager_Initialize(const char* pvf_path, const char* stringtable_path)
{
    int ret;

    if (g_pvf_state.initialized) {
        return PVF_MGR_ERROR_ALREADY_INIT;
    }

    if (!pvf_path) {
        return PVF_MGR_ERROR_INVALID_PARAM;
    }

    /* 初始化CRC32表 */
    PVF_Initialize();

    /* 初始化PackSet结构 */
    memset(&g_pvf_state.pack, 0, sizeof(g_pvf_state.pack));

    /* 打开PVF文件 */
    ret = PackSet_Open(pvf_path, &g_pvf_state.pack);
    if (ret != PVF_SUCCESS) {
        fprintf(stderr, "[PVF] Failed to open: %s (error: %d)\n", pvf_path, ret);
        return PVF_MGR_ERROR_FILE_NOT_FOUND;
    }

    strncpy(g_pvf_state.pvf_path, pvf_path, sizeof(g_pvf_state.pvf_path) - 1);

    /* 加载StringTable（可选） */
    g_pvf_state.stringtable_loaded = 0;
    if (stringtable_path) {
        StringTable_Init(&g_pvf_state.stringtable);
        ret = StringTable_LoadFromFile(&g_pvf_state.stringtable, stringtable_path);
        if (ret == STRINGTABLE_SUCCESS) {
            g_pvf_state.stringtable_loaded = 1;
            strncpy(g_pvf_state.stringtable_path, stringtable_path,
                    sizeof(g_pvf_state.stringtable_path) - 1);
            printf("[PVF] StringTable loaded: %u entries\n", g_pvf_state.stringtable.count);
        } else {
            fprintf(stderr, "[PVF] Warning: StringTable not loaded: %s (error: %d)\n",
                    stringtable_path, ret);
        }
    }

    g_pvf_state.initialized = 1;

    printf("[PVF] Manager initialized: %u files, revision %u\n",
           g_pvf_state.pack.header.index_count,
           g_pvf_state.pack.header.revision);

    return PVF_MGR_SUCCESS;
}

void PVF_Manager_Cleanup(void)
{
    if (!g_pvf_state.initialized) {
        return;
    }

    /* 关闭PackSet（内部会销毁hash表） */
    PackSet_Close(&g_pvf_state.pack);

    /* 释放StringTable */
    if (g_pvf_state.stringtable_loaded) {
        StringTable_Free(&g_pvf_state.stringtable);
        g_pvf_state.stringtable_loaded = 0;
    }

    memset(&g_pvf_state, 0, sizeof(g_pvf_state));

    printf("[PVF] Manager cleanup complete\n");
}

int PVF_Manager_IsInitialized(void)
{
    return g_pvf_state.initialized;
}

/* ========================================================================== */
/* 状态查询                                                                    */
/* ========================================================================== */

uint32_t PVF_Manager_GetFileCount(void)
{
    if (!g_pvf_state.initialized) {
        return 0;
    }
    return g_pvf_state.pack.header.index_count;
}

uint32_t PVF_Manager_GetRevision(void)
{
    if (!g_pvf_state.initialized) {
        return 0;
    }
    return g_pvf_state.pack.header.revision;
}

/* ========================================================================== */
/* 文件访问                                                                    */
/* ========================================================================== */

int PVF_Manager_FileExists(const char* filepath)
{
    if (!g_pvf_state.initialized || !filepath) {
        return 0;
    }

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_state.pack, filepath);
    return (entry != NULL) ? 1 : 0;
}

int32_t PVF_Manager_GetFileSize(const char* filepath)
{
    if (!g_pvf_state.initialized || !filepath) {
        return -1;
    }

    PVFIndexEntry* entry = PackSet_GetFile(&g_pvf_state.pack, filepath);
    if (!entry) {
        return -1;
    }

    return (int32_t)entry->size;
}

int PVF_Manager_ReadFile(const char* filepath, unsigned char** buffer, size_t* size)
{
    PVFIndexEntry* entry;
    int ret;
    unsigned char* data;

    if (!g_pvf_state.initialized) {
        return PVF_MGR_ERROR_NOT_INIT;
    }

    if (!filepath || !buffer || !size) {
        return PVF_MGR_ERROR_INVALID_PARAM;
    }

    /* 查找文件 */
    entry = PackSet_GetFile(&g_pvf_state.pack, filepath);
    if (!entry) {
        return PVF_MGR_ERROR_FILE_NOT_FOUND;
    }

    /* 分配缓冲区 */
    data = (unsigned char*)malloc(entry->size);
    if (!data) {
        return PVF_MGR_ERROR_LOAD_FAILED;
    }

    /* 提取文件内容 */
    ret = PackSet_ExtractFile(&g_pvf_state.pack, entry, data, entry->size);
    if (ret != PVF_SUCCESS) {
        free(data);
        return PVF_MGR_ERROR_LOAD_FAILED;
    }

    *buffer = data;
    *size = entry->size;

    return PVF_MGR_SUCCESS;
}

/* ========================================================================== */
/* StringTable访问                                                             */
/* ========================================================================== */

const char* PVF_Manager_GetString(uint32_t index)
{
    if (!g_pvf_state.initialized || !g_pvf_state.stringtable_loaded) {
        return NULL;
    }
    return StringTable_GetString(&g_pvf_state.stringtable, index);
}

int PVF_Manager_GetTag(uint32_t index, char* tag_buf, size_t buf_size)
{
    if (!g_pvf_state.initialized || !g_pvf_state.stringtable_loaded) {
        return -1;
    }
    return StringTable_GetTag(&g_pvf_state.stringtable, index, tag_buf, buf_size);
}

/* ========================================================================== */
/* 数据解析                                                                    */
/* ========================================================================== */

int PVF_Manager_ParseEquipment(const char* filepath, void* out_data)
{
    /*
     * Equipment parsing: Extract equipment data from PVF file
     *
     * Implementation flow:
     *   1. Extract raw file data:
     *      uint8_t* data = NULL;
     *      size_t size = 0;
     *      if (PVF_Manager_ExtractFile(filepath, &data, &size) != 0) {
     *          return PVF_MGR_ERROR_FILE_NOT_FOUND;
     *      }
     *
     *   2. Parse using equipment_parser:
     *      EquipmentData* equip = (EquipmentData*)out_data;
     *      if (EquipmentParser_Parse(data, size, equip) != 0) {
     *          free(data);
     *          return PVF_MGR_ERROR_PARSE_FAILED;
     *      }
     *
     *   3. Cleanup:
     *      free(data);
     *      return 0;
     *
     * Equipment files are in: equipment/character/*.equ
     * See: src/pvf/parsers/equipment_parser.h for data structures
     */
    (void)filepath;
    (void)out_data;

    if (!g_pvf_state.initialized) {
        return PVF_MGR_ERROR_NOT_INIT;
    }

    /* Parser integration pending - return not implemented */
    printf("[PVF] Equipment parsing not yet integrated: %s\n", filepath);
    return PVF_MGR_ERROR_NOT_INIT;
}

/* ========================================================================== */
/* 诊断                                                                        */
/* ========================================================================== */

void PVF_Manager_PrintStatus(void)
{
    printf("=== PVF Manager Status ===\n");

    if (!g_pvf_state.initialized) {
        printf("Status: NOT INITIALIZED\n");
        return;
    }

    printf("Status:      INITIALIZED\n");
    printf("PVF Path:    %s\n", g_pvf_state.pvf_path);
    printf("Revision:    %u\n", g_pvf_state.pack.header.revision);
    printf("File Count:  %u\n", g_pvf_state.pack.header.index_count);
    printf("StringTable: %s\n", g_pvf_state.stringtable_loaded ? "LOADED" : "NOT LOADED");

    if (g_pvf_state.stringtable_loaded) {
        printf("  Path:      %s\n", g_pvf_state.stringtable_path);
        printf("  Entries:   %u\n", g_pvf_state.stringtable.count);
    }

    printf("==========================\n");
}
