/**
 * @file pvf_api.c
 * @brief PVF浏览器后端API - 提供JSON格式的数据
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvf/pvf.h"
#include "pvf/parsers/equipment_parser.h"

/* 全局PVF包 */
static PackSetInternal g_pack;
static int g_initialized = 0;

/**
 * @brief 初始化PVF
 */
int api_init(const char* pvf_path) {
    if (g_initialized) {
        PackSet_Close(&g_pack);
    }

    PVF_Initialize();

    if (PackSet_Open(pvf_path, &g_pack) != PVF_SUCCESS) {
        return -1;
    }

    g_initialized = 1;
    return 0;
}

/**
 * @brief 输出JSON响应头
 */
void json_header() {
    printf("Content-Type: application/json; charset=utf-8\n\n");
}

/**
 * @brief 输出JSON错误
 */
void json_error(const char* message) {
    json_header();
    printf("{\"success\":false,\"error\":\"%s\"}\n", message);
}

/**
 * @brief 转义JSON字符串
 */
void json_escape_string(const char* str) {
    while (*str) {
        switch (*str) {
            case '"':  printf("\\\""); break;
            case '\\': printf("\\\\"); break;
            case '\n': printf("\\n"); break;
            case '\r': printf("\\r"); break;
            case '\t': printf("\\t"); break;
            default:   putchar(*str); break;
        }
        str++;
    }
}

/**
 * @brief API: 列出目录
 *
 * @param path 目录路径（如 "equipment/", 空字符串表示根目录）
 * @param type 文件类型过滤（"equ", "stk", "all"）
 */
void api_list_directory(const char* path, const char* type) {
    int i;
    int count = 0;
    int is_first = 1;

    json_header();
    printf("{\"success\":true,\"path\":\"%s\",\"items\":[\n", path);

    /* 遍历索引 */
    for (i = 0; i < (int)g_pack.header.index_count; i++) {
        char* name = g_pack.indexes[i].name;

        /* 路径过滤 */
        if (path && strlen(path) > 0) {
            if (strncmp(name, path, strlen(path)) != 0) {
                continue;
            }
        }

        /* 类型过滤 */
        if (strcmp(type, "all") != 0) {
            char ext[16];
            snprintf(ext, sizeof(ext), ".%s", type);
            if (strstr(name, ext) == NULL) {
                continue;
            }
        }

        /* 输出文件信息 */
        if (!is_first) printf(",\n");
        is_first = 0;

        printf("  {\"name\":\"");
        json_escape_string(name);
        printf("\",\"size\":%u,\"hash\":\"0x%08X\"}",
               g_pack.indexes[i].size,
               g_pack.indexes[i].hash_value);

        count++;

        /* 限制返回数量 */
        if (count >= 1000) {
            printf(",\n  {\"warning\":\"结果超过1000条，已截断\"}");
            break;
        }
    }

    printf("\n],\"count\":%d}\n", count);
}

/**
 * @brief API: 获取文件统计信息
 */
void api_get_statistics() {
    int i;
    int equ_count = 0;
    int stk_count = 0;
    int ani_count = 0;
    int other_count = 0;

    for (i = 0; i < (int)g_pack.header.index_count; i++) {
        char* name = g_pack.indexes[i].name;

        if (strstr(name, ".equ")) equ_count++;
        else if (strstr(name, ".stk")) stk_count++;
        else if (strstr(name, ".ani")) ani_count++;
        else other_count++;
    }

    json_header();
    printf("{\n");
    printf("  \"success\":true,\n");
    printf("  \"total_files\":%u,\n", g_pack.header.index_count);
    printf("  \"equipment\":%d,\n", equ_count);
    printf("  \"stackable\":%d,\n", stk_count);
    printf("  \"animation\":%d,\n", ani_count);
    printf("  \"other\":%d\n", other_count);
    printf("}\n");
}

/**
 * @brief API: 解析装备文件
 */
void api_parse_equipment(const char* filepath) {
    EquipmentData eq;
    int result;

    result = Equipment_ParseFromPVF(&eq, &g_pack, filepath);
    if (result != PVF_SUCCESS) {
        json_error("解析装备失败");
        return;
    }

    json_header();
    printf("{\n");
    printf("  \"success\":true,\n");
    printf("  \"filepath\":\"");
    json_escape_string(filepath);
    printf("\",\n");

    printf("  \"basic\":{\n");
    printf("    \"item_id\":\"0x%08X\",\n", eq.item_id);
    printf("    \"name_ref\":\"0x%08X\",\n", eq.name_ref);
    printf("    \"grade\":%d,\n", eq.grade);
    printf("    \"rarity\":%d,\n", eq.rarity);
    printf("    \"min_level\":%d,\n", eq.min_level);
    printf("    \"weight\":%d\n", eq.weight);
    printf("  },\n");

    printf("  \"stats\":{\n");
    printf("    \"strength\":%d,\n", eq.strength);
    printf("    \"intelligence\":%d,\n", eq.intelligence);
    printf("    \"stamina\":%d,\n", eq.stamina);
    printf("    \"spirit\":%d,\n", eq.spirit);
    printf("    \"hp_max\":%d,\n", eq.hp_max);
    printf("    \"mp_max\":%d\n", eq.mp_max);
    printf("  },\n");

    printf("  \"defense\":{\n");
    printf("    \"physical\":%d,\n", eq.physical_defense);
    printf("    \"magical\":%d,\n", eq.magical_defense);
    printf("    \"equip_physical_min\":%d,\n", eq.equip_physical_def_min);
    printf("    \"equip_physical_max\":%d\n", eq.equip_physical_def_max);
    printf("  },\n");

    printf("  \"regen\":{\n");
    printf("    \"hp\":%d,\n", eq.hp_regen);
    printf("    \"mp\":%d\n", eq.mp_regen);
    printf("  },\n");

    printf("  \"special\":{\n");
    printf("    \"anti_evil\":%d,\n", eq.anti_evil);
    printf("    \"physical_crit\":%d,\n", eq.physical_crit);
    printf("    \"magical_crit\":%d\n", eq.magical_crit);
    printf("  },\n");

    printf("  \"speed\":{\n");
    printf("    \"move\":%d,\n", eq.move_speed);
    printf("    \"attack\":%d,\n", eq.cast_speed);
    printf("    \"cast\":%d\n", eq.cast_speed);
    printf("  },\n");

    printf("  \"price\":{\n");
    printf("    \"base\":%d,\n", eq.price);
    printf("    \"repair\":%d,\n", eq.repair_price);
    printf("    \"sell\":%d\n", eq.sell_price);
    printf("  },\n");

    printf("  \"other\":{\n");
    printf("    \"cool_time\":%d,\n", eq.cool_time);
    printf("    \"inventory_limit\":%d,\n", eq.inventory_limit);
    printf("    \"raw_data_count\":%d\n", eq.raw_data.count);
    printf("  }\n");

    printf("}\n");

    Equipment_Free(&eq);
}

/**
 * @brief API: 搜索装备
 */
void api_search_equipment(const char* keyword, const char* criteria) {
    int i;
    int count = 0;
    int is_first = 1;

    json_header();
    printf("{\"success\":true,\"keyword\":\"%s\",\"results\":[\n", keyword);

    for (i = 0; i < (int)g_pack.header.index_count && count < 100; i++) {
        char* name = g_pack.indexes[i].name;

        if (strstr(name, ".equ") == NULL) continue;

        /* 简单的名称匹配 */
        if (keyword && strlen(keyword) > 0) {
            if (strstr(name, keyword) == NULL) continue;
        }

        /* 解析装备并根据条件过滤 */
        EquipmentData eq;
        if (Equipment_ParseFromPVF(&eq, &g_pack, name) != PVF_SUCCESS) {
            continue;
        }

        int match = 1;

        /* 根据条件过滤 */
        if (strcmp(criteria, "epic") == 0 && eq.rarity != 5) match = 0;
        if (strcmp(criteria, "legendary") == 0 && eq.rarity != 4) match = 0;
        if (strcmp(criteria, "high_level") == 0 && eq.min_level < 70) match = 0;

        if (match) {
            if (!is_first) printf(",\n");
            is_first = 0;

            printf("  {\"path\":\"");
            json_escape_string(name);
            printf("\",\"grade\":%d,\"rarity\":%d,\"level\":%d}",
                   eq.grade, eq.rarity, eq.min_level);
            count++;
        }

        Equipment_Free(&eq);
    }

    printf("\n],\"count\":%d}\n", count);
}

/**
 * @brief 主函数 - CGI模式
 */
int main(int argc, char* argv[]) {
    char* query_string;
    char* action = NULL;
    char* pvf_path = NULL;
    char* path = NULL;
    char* type = "all";
    char* filepath = NULL;
    char* keyword = NULL;
    char* criteria = "all";

    /* 从环境变量或命令行获取参数 */
    if (argc >= 2) {
        /* 命令行模式：./pvf_api action pvf_path [params...] */
        action = argv[1];

        if (argc >= 3) pvf_path = argv[2];
        if (argc >= 4) path = argv[3];
        if (argc >= 5) type = argv[4];
        if (argc >= 4) filepath = argv[3];
        if (argc >= 4) keyword = argv[3];
        if (argc >= 5) criteria = argv[4];
    } else {
        /* CGI模式：从QUERY_STRING解析 */
        query_string = getenv("QUERY_STRING");
        if (query_string == NULL) {
            json_error("缺少QUERY_STRING");
            return 1;
        }

        /* 简单的参数解析 */
        char* token = strtok(query_string, "&");
        while (token != NULL) {
            if (strncmp(token, "action=", 7) == 0) action = token + 7;
            else if (strncmp(token, "pvf=", 4) == 0) pvf_path = token + 4;
            else if (strncmp(token, "path=", 5) == 0) path = token + 5;
            else if (strncmp(token, "type=", 5) == 0) type = token + 5;
            else if (strncmp(token, "file=", 5) == 0) filepath = token + 5;
            else if (strncmp(token, "keyword=", 8) == 0) keyword = token + 8;
            else if (strncmp(token, "criteria=", 9) == 0) criteria = token + 9;

            token = strtok(NULL, "&");
        }
    }

    /* 初始化PVF */
    if (pvf_path == NULL) {
        pvf_path = "../../Script.pvf";  /* 默认路径 */
    }

    if (api_init(pvf_path) != 0) {
        json_error("无法打开PVF文件");
        return 1;
    }

    /* 执行对应的API */
    if (action == NULL) {
        json_error("缺少action参数");
    }
    else if (strcmp(action, "list") == 0) {
        api_list_directory(path ? path : "", type);
    }
    else if (strcmp(action, "stats") == 0) {
        api_get_statistics();
    }
    else if (strcmp(action, "parse") == 0) {
        if (filepath == NULL) {
            json_error("缺少file参数");
        } else {
            api_parse_equipment(filepath);
        }
    }
    else if (strcmp(action, "search") == 0) {
        api_search_equipment(keyword ? keyword : "", criteria);
    }
    else {
        json_error("未知的action");
    }

    /* 清理 */
    if (g_initialized) {
        PackSet_Close(&g_pack);
    }

    return 0;
}
