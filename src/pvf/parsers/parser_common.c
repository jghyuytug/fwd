/**
 * @file parser_common.c
 * @brief PVF数据解析器通用实现
 *
 * @date 2025-11-25
 */

#include "pvf/parsers/parser_common.h"
#include "pvf/pvf_data_parser.h"
#include "pvf/pvf_stringtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* StringTable 全局实例                                                        */
/* ========================================================================== */

static PVFStringTable* g_stringtable = NULL;

/* ========================================================================== */
/* 全局变量                                                                    */
/* ========================================================================== */

/* 类型名称映射表 */
static const char* TYPE_NAMES[PVF_DATA_TYPE_MAX] = {
    "unknown",
    "equipment",
    "stackable",
    "skill",
    "monster",
    "dungeon",
    "npc",
    "quest",
    "shop",
    "animation",
    "attack",
    "action",
    "passive_object",
    "character",
    "ui",
    "aura",
    "particle",
    "pet"
};

/* 稀有度名称映射表 */
static const char* RARITY_NAMES[] = {
    "\xE6\x99\xAE\xE9\x80\x9A",     /* 普通 */
    "\xE9\xAB\x98\xE7\xBA\xA7",     /* 高级 */
    "\xE7\xA8\x80\xE6\x9C\x89",     /* 稀有 */
    "\xE7\xA5\x9E\xE5\x99\xA8",     /* 神器 */
    "\xE4\xBC\xA0\xE8\xAF\xB4",     /* 传说 */
    "\xE5\x8F\xB2\xE8\xAF\x97"      /* 史诗 */
};

/* 类型特定解析器注册表 */
static TypeSpecificParser g_parsers[PVF_DATA_TYPE_MAX] = {NULL};

/* ========================================================================== */
/* 基础函数实现                                                                */
/* ========================================================================== */

void PVFData_Init(PVFDataGeneric* data, PVFDataType data_type) {
    if (data == NULL) {
        return;
    }

    /* 清空整个结构体 */
    memset(data, 0, sizeof(PVFDataGeneric));

    /* 设置数据类型 */
    data->data_type = data_type;
    if (data_type >= 0 && data_type < PVF_DATA_TYPE_MAX) {
        strncpy(data->type_name, TYPE_NAMES[data_type], PVF_TYPE_MAX_LENGTH - 1);
    }

    /* 设置默认值 */
    data->grade = -1;
    data->rarity = -1;
    data->min_level = 0;
    data->price = 0;
    data->weight = 0;
}

int PVFData_Parse(PVFDataGeneric* data, const void* buffer, size_t size) {
    int ret;

    if (data == NULL || buffer == NULL || size == 0) {
        return PVF_ERROR_INVALID_ARGUMENT;
    }

    /* 解析PVF容器 */
    ret = PVFDataContainer_Parse(&data->raw_data, buffer, size);
    if (ret != PVF_SUCCESS) {
        return ret;
    }

    /* 提取通用字段 - 使用 stringtable 中验证的正确键值 */

    /* 名称 (key: TAG_NAME = 0x00002ca5 = [name]) */
    /*
     * String extraction: Name field is encoded as length-prefixed UTF-8
     *
     * Implementation:
     *   const char* name_str = PVFDataContainer_GetString(&data->raw_data, TAG_NAME, NULL);
     *   if (name_str) {
     *       strncpy(data->name, name_str, sizeof(data->name) - 1);
     *       data->name[sizeof(data->name) - 1] = '\0';
     *   }
     *
     * Note: PVFDataContainer_GetString needs to handle:
     * - 2-byte length prefix (little endian)
     * - UTF-8 encoded string data
     * - Null termination
     *
     * Current: Name extraction deferred (integer fields prioritized)
     */

    /* 品级 (key: TAG_GRADE = 0x00004817 = [grade]) */
    data->grade = PVFDataContainer_GetInt(&data->raw_data, TAG_GRADE, -1);

    /* 稀有度 (key: TAG_RARITY = 0x0000066c = [rarity]) */
    data->rarity = PVFDataContainer_GetInt(&data->raw_data, TAG_RARITY, -1);

    /* 最低等级 (key: TAG_MINIMUM_LEVEL = 0x0000481c = [minimum level]) */
    data->min_level = PVFDataContainer_GetInt(&data->raw_data, TAG_MINIMUM_LEVEL, 0);

    /* 价格 (key: TAG_PRICE = 0x0000481d = [price]) */
    data->price = PVFDataContainer_GetInt(&data->raw_data, TAG_PRICE, 0);

    /* 重量 (key: TAG_WEIGHT = 0x00001d14 = [weight]) */
    data->weight = PVFDataContainer_GetInt(&data->raw_data, TAG_WEIGHT, 0);

    /* 调用类型特定解析器（如果已注册） */
    if (data->data_type >= 0 && data->data_type < PVF_DATA_TYPE_MAX) {
        TypeSpecificParser parser = g_parsers[data->data_type];
        if (parser != NULL) {
            ret = parser(data);
            if (ret != PVF_SUCCESS) {
                return ret;
            }
        }
    }

    return PVF_SUCCESS;
}

int PVFData_ParseFromPVF(PVFDataGeneric* data, PackSetInternal* pack, const char* filepath) {
    PVFIndexEntry* entry;
    void* buffer;
    int ret;

    if (data == NULL || pack == NULL || filepath == NULL) {
        return PVF_ERROR_INVALID_ARGUMENT;
    }

    /* 保存文件路径（用于调试） */
    strncpy(data->file_path, filepath, PVF_NAME_MAX_LENGTH - 1);

    /* 查找文件 */
    entry = PackSet_GetFile(pack, filepath);
    if (entry == NULL) {
        return PVF_ERROR_FILE_NOT_FOUND;
    }

    /* 分配缓冲区 */
    buffer = malloc(entry->size);
    if (buffer == NULL) {
        return PVF_ERROR_OUT_OF_MEMORY;
    }

    /* 提取文件内容 */
    ret = PackSet_ExtractFile(pack, entry, buffer, entry->size);
    if (ret != PVF_SUCCESS) {
        free(buffer);
        return ret;
    }

    /* 解析数据 */
    ret = PVFData_Parse(data, buffer, entry->size);

    /* 释放缓冲区 */
    free(buffer);

    return ret;
}

void PVFData_Print(const PVFDataGeneric* data) {
    if (data == NULL) {
        return;
    }

    printf("========== %s ==========\n", data->type_name);
    printf("File Path:    %s\n", data->file_path);
    printf("Name:         %s\n", data->name[0] ? data->name : "(empty)");
    printf("Item ID:      0x%08X\n", data->item_id);
    printf("Grade:        %d\n", data->grade);
    printf("Rarity:       %d (%s)\n", data->rarity, PVFData_GetRarityName(data->rarity));
    printf("Min Level:    %d\n", data->min_level);
    printf("Price:        %d\n", data->price);
    printf("Weight:       %d g\n", data->weight);

    if (data->basic_explain[0]) {
        printf("Explanation:  %s\n", data->basic_explain);
    }

    printf("Raw Elements: %d\n", data->raw_data.count);
    printf("========================================\n");
}

void PVFData_Free(PVFDataGeneric* data) {
    if (data == NULL) {
        return;
    }

    /* 释放原始容器 */
    PVFDataContainer_Free(&data->raw_data);

    /* 释放类型特定数据 */
    if (data->type_specific_data != NULL) {
        free(data->type_specific_data);
        data->type_specific_data = NULL;
    }

    /* 清空结构体 */
    memset(data, 0, sizeof(PVFDataGeneric));
}

int PVFData_ToJSON(const PVFDataGeneric* data, char* json_buffer, size_t buffer_size) {
    int written = 0;
    char temp[512];

    if (data == NULL || json_buffer == NULL || buffer_size == 0) {
        return -1;
    }

    /* 开始JSON对象 */
    written += snprintf(json_buffer + written, buffer_size - written, "{\n");

    /* 类型 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"type\": \"%s\",\n", data->type_name);

    /* 文件路径 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"file_path\": \"%s\",\n", data->file_path);

    /* 名称 */
    if (data->name[0]) {
        written += snprintf(json_buffer + written, buffer_size - written,
                            "  \"name\": \"%s\",\n", data->name);
    }

    /* ItemID */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"item_id\": \"0x%08X\",\n", data->item_id);

    /* 品级 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"grade\": %d,\n", data->grade);

    /* 稀有度 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"rarity\": %d,\n", data->rarity);
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"rarity_name\": \"%s\",\n", PVFData_GetRarityName(data->rarity));

    /* 最低等级 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"min_level\": %d,\n", data->min_level);

    /* 价格 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"price\": %d,\n", data->price);

    /* 重量 */
    written += snprintf(json_buffer + written, buffer_size - written,
                        "  \"weight\": %d\n", data->weight);

    /* 结束JSON对象 */
    written += snprintf(json_buffer + written, buffer_size - written, "}\n");

    return written;
}

/* ========================================================================== */
/* 工具函数实现                                                                */
/* ========================================================================== */

PVFDataType PVFData_DetectType(const char* filepath) {
    const char* ext;

    if (filepath == NULL) {
        return PVF_DATA_TYPE_UNKNOWN;
    }

    /* 查找扩展名 */
    ext = strrchr(filepath, '.');
    if (ext == NULL) {
        return PVF_DATA_TYPE_UNKNOWN;
    }

    /* 跳过点号 */
    ext++;

    /* 匹配扩展名 */
    if (strcmp(ext, "equ") == 0) return PVF_DATA_TYPE_EQUIPMENT;
    if (strcmp(ext, "stk") == 0) return PVF_DATA_TYPE_STACKABLE;
    if (strcmp(ext, "skl") == 0) return PVF_DATA_TYPE_SKILL;
    if (strcmp(ext, "mob") == 0) return PVF_DATA_TYPE_MONSTER;
    if (strcmp(ext, "dgn") == 0) return PVF_DATA_TYPE_DUNGEON;
    if (strcmp(ext, "npc") == 0) return PVF_DATA_TYPE_NPC;
    if (strcmp(ext, "qst") == 0) return PVF_DATA_TYPE_QUEST;
    if (strcmp(ext, "shp") == 0) return PVF_DATA_TYPE_SHOP;
    if (strcmp(ext, "ani") == 0) return PVF_DATA_TYPE_ANIMATION;
    if (strcmp(ext, "atk") == 0) return PVF_DATA_TYPE_ATTACK;
    if (strcmp(ext, "act") == 0) return PVF_DATA_TYPE_ACTION;
    if (strcmp(ext, "obj") == 0) return PVF_DATA_TYPE_PASSIVE_OBJECT;
    if (strcmp(ext, "chr") == 0) return PVF_DATA_TYPE_CHARACTER;
    if (strcmp(ext, "ui") == 0) return PVF_DATA_TYPE_UI;
    if (strcmp(ext, "ora") == 0) return PVF_DATA_TYPE_AURA;
    if (strcmp(ext, "ptl") == 0) return PVF_DATA_TYPE_PARTICLE;
    if (strcmp(ext, "pet") == 0) return PVF_DATA_TYPE_PET;

    return PVF_DATA_TYPE_UNKNOWN;
}

const char* PVFData_GetTypeName(PVFDataType data_type) {
    if (data_type >= 0 && data_type < PVF_DATA_TYPE_MAX) {
        return TYPE_NAMES[data_type];
    }
    return "unknown";
}

const char* PVFData_GetRarityName(int32_t rarity) {
    if (rarity >= 0 && rarity < 6) {
        return RARITY_NAMES[rarity];
    }
    return "unknown";
}

int PVFData_Validate(const PVFDataGeneric* data) {
    if (data == NULL) {
        return 0;
    }

    /* 检查数据类型 */
    if (data->data_type <= PVF_DATA_TYPE_UNKNOWN || data->data_type >= PVF_DATA_TYPE_MAX) {
        return 0;
    }

    /* 检查稀有度范围 */
    if (data->rarity < -1 || data->rarity > 5) {
        return 0;
    }

    /* 检查品级范围 */
    if (data->grade < -1 || data->grade > 100) {
        return 0;
    }

    /* 检查最低等级范围 */
    if (data->min_level < 0 || data->min_level > 100) {
        return 0;
    }

    return 1;
}

/* ========================================================================== */
/* 解析器注册机制                                                              */
/* ========================================================================== */

void PVFData_RegisterParser(PVFDataType data_type, TypeSpecificParser parser) {
    if (data_type >= 0 && data_type < PVF_DATA_TYPE_MAX) {
        g_parsers[data_type] = parser;
    }
}

/* ========================================================================== */
/* StringTable 集成函数                                                        */
/* ========================================================================== */

/**
 * @brief 初始化 StringTable（从文件加载）
 *
 * @param filepath  stringtable.bin 文件路径
 *
 * @return int 0=成功, 其他=错误码
 */
int PVFData_InitStringTable(const char* filepath) {
    int ret;

    if (filepath == NULL) {
        return STRINGTABLE_ERROR_NULL_PTR;
    }

    /* 分配 StringTable 结构体 */
    if (g_stringtable == NULL) {
        g_stringtable = (PVFStringTable*)malloc(sizeof(PVFStringTable));
        if (g_stringtable == NULL) {
            return STRINGTABLE_ERROR_OUT_OF_MEM;
        }
        StringTable_Init(g_stringtable);
    }

    /* 加载文件 */
    ret = StringTable_LoadFromFile(g_stringtable, filepath);
    if (ret != STRINGTABLE_SUCCESS) {
        free(g_stringtable);
        g_stringtable = NULL;
        return ret;
    }

    return STRINGTABLE_SUCCESS;
}

/**
 * @brief 释放 StringTable
 */
void PVFData_FreeStringTable(void) {
    if (g_stringtable != NULL) {
        StringTable_Free(g_stringtable);
        free(g_stringtable);
        g_stringtable = NULL;
    }
}

/**
 * @brief 检查 StringTable 是否已加载
 *
 * @return int 1=已加载, 0=未加载
 */
int PVFData_IsStringTableLoaded(void) {
    return (g_stringtable != NULL && g_stringtable->loaded);
}

/**
 * @brief 根据键获取标签名
 *
 * @param key       键值（stringtable 索引）
 * @param tag_buf   输出缓冲区
 * @param buf_size  缓冲区大小
 *
 * @return int 成功返回标签长度，失败返回负数
 *
 * @example
 *     char tag[128];
 *     PVFData_GetTagName(0x00004817, tag, sizeof(tag));
 *     // tag = "[grade]"
 */
int PVFData_GetTagName(uint32_t key, char* tag_buf, size_t buf_size) {
    if (g_stringtable == NULL || !g_stringtable->loaded) {
        /* 如果 StringTable 未加载，返回十六进制键 */
        if (tag_buf != NULL && buf_size > 0) {
            snprintf(tag_buf, buf_size, "0x%08x", key);
            return (int)strlen(tag_buf);
        }
        return STRINGTABLE_ERROR_NOT_LOADED;
    }

    return StringTable_GetTag(g_stringtable, key, tag_buf, buf_size);
}

/**
 * @brief 打印 PVF 数据的详细内容（带标签名解析）
 *
 * @param data  PVFDataGeneric 结构体指针
 */
void PVFData_PrintDetailed(const PVFDataGeneric* data) {
    int i;
    char tag_buf[STRINGTABLE_MAX_TAG_LENGTH];

    if (data == NULL) {
        return;
    }

    printf("========== %s (Detailed) ==========\n", data->type_name);
    printf("File Path:    %s\n", data->file_path);
    printf("Name:         %s\n", data->name[0] ? data->name : "(empty)");
    printf("Item ID:      0x%08X\n", data->item_id);
    printf("Grade:        %d\n", data->grade);
    printf("Rarity:       %d (%s)\n", data->rarity, PVFData_GetRarityName(data->rarity));
    printf("Min Level:    %d\n", data->min_level);
    printf("Price:        %d\n", data->price);
    printf("Weight:       %d g\n", data->weight);

    if (data->basic_explain[0]) {
        printf("Explanation:  %s\n", data->basic_explain);
    }

    printf("\n--- Raw Entries (%d) ---\n", data->raw_data.count);

    /* 遍历所有原始条目，解析标签名 */
    for (i = 0; i < data->raw_data.count && i < 50; i++) {
        PVFDataEntry* entry = &data->raw_data.entries[i];

        /* 获取标签名 */
        if (PVFData_GetTagName(entry->key, tag_buf, sizeof(tag_buf)) > 0) {
            printf("  [%2d] %s (0x%08x) = ", i, tag_buf, entry->key);
        } else {
            printf("  [%2d] 0x%08x = ", i, entry->key);
        }

        /* 根据类型输出值 */
        switch (entry->type) {
            case PVF_VALUE_TYPE_INTEGER:
                printf("%d\n", entry->value.int_value);
                break;
            case PVF_VALUE_TYPE_REFERENCE:
                printf("ref:0x%08x\n", entry->value.ref_value);
                break;
            case PVF_VALUE_TYPE_KV:
                printf("kv:{0x%x, 0x%x}\n", entry->value.kv.sub_key, entry->value.kv.sub_value);
                break;
            default:
                printf("(type 0x%02x)\n", entry->type);
                break;
        }
    }

    if (data->raw_data.count > 50) {
        printf("  ... (%d more entries)\n", data->raw_data.count - 50);
    }

    printf("========================================\n");
}
