#include "charac_dungeon_test_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_HEX_LEN 16384

/* 辅助函数：将二进制数据转换为十六进制字符串 */
static char* BinaryToHex(const unsigned char* data, unsigned int len) {
    if (!data || len == 0) {
        return NULL;
    }

    char* hex = (char*)malloc(len * 2 + 1);
    if (!hex) {
        return NULL;
    }

    for (unsigned int i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02x", data[i]);
    }
    hex[len * 2] = '\0';

    return hex;
}

/* 辅助函数：将十六进制字符串转换为二进制数据 */
static unsigned char* HexToBinary(const char* hex, unsigned int* out_len) {
    if (!hex) {
        return NULL;
    }

    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0) {
        return NULL;
    }

    unsigned int bin_len = hex_len / 2;
    unsigned char* bin = (unsigned char*)malloc(bin_len);
    if (!bin) {
        return NULL;
    }

    for (unsigned int i = 0; i < bin_len; i++) {
        sscanf(hex + i * 2, "%2hhx", &bin[i]);
    }

    *out_len = bin_len;
    return bin;
}

/* CRUD 操作实现 */
int CharacDungeonTest_Add(DBConnectionManager* manager, const CharacDungeonTest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* hex_dungeon = NULL;

    if (!manager || !record) {
        return -1;
    }

    /* 转换 blob 数据为十六进制 */
    if (record->dungeon && record->dungeon_len > 0) {
        hex_dungeon = BinaryToHex(record->dungeon, record->dungeon_len);
        if (!hex_dungeon) {
            return -1;
        }
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_dungeon_test (charac_no, dungeon) VALUES (%u, UNHEX('%s'))",
        record->charac_no,
        hex_dungeon ? hex_dungeon : "");

    free(hex_dungeon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeonTest_Get(DBConnectionManager* manager, unsigned int charac_no, CharacDungeonTest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[2];
    int ret;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(dungeon) as hex_dungeon FROM charac_dungeon_test WHERE charac_no = %u",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->charac_no = atoi(values[0]);

    /* 解析十六进制 blob 数据 */
    if (values[1] && strlen(values[1]) > 0) {
        record->dungeon = HexToBinary(values[1], &record->dungeon_len);
    } else {
        record->dungeon = NULL;
        record->dungeon_len = 0;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeonTest_Update(DBConnectionManager* manager, const CharacDungeonTest* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* hex_dungeon = NULL;

    if (!manager || !record) {
        return -1;
    }

    /* 转换 blob 数据为十六进制 */
    if (record->dungeon && record->dungeon_len > 0) {
        hex_dungeon = BinaryToHex(record->dungeon, record->dungeon_len);
        if (!hex_dungeon) {
            return -1;
        }
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_dungeon_test SET dungeon = UNHEX('%s') WHERE charac_no = %u",
        hex_dungeon ? hex_dungeon : "",
        record->charac_no);

    free(hex_dungeon);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeonTest_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_dungeon_test WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeonTest_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int exists = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_dungeon_test WHERE charac_no = %u",
        charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* 查询操作 */
int CharacDungeonTest_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_dungeon_test");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacDungeonTest_GetAll(DBConnectionManager* manager, CharacDungeonTest** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[2];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(dungeon) FROM charac_dungeon_test ORDER BY charac_no");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *count = result.row_count;
    if (*count == 0) {
        *records = NULL;
        DBQueryResult_Free(&result);
        return 0;
    }

    *records = (CharacDungeonTest*)malloc(sizeof(CharacDungeonTest) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].charac_no = atoi(values[0]);

        if (values[1] && strlen(values[1]) > 0) {
            (*records)[i].dungeon = HexToBinary(values[1], &(*records)[i].dungeon_len);
        } else {
            (*records)[i].dungeon = NULL;
            (*records)[i].dungeon_len = 0;
        }
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 批量操作 */
int CharacDungeonTest_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM charac_dungeon_test");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacDungeonTest_BatchDelete(DBConnectionManager* manager, const unsigned int* charac_nos, int count) {
    char query[MAX_QUERY_LEN];
    char ids[4096];
    DBQueryResult result;

    if (!manager || !charac_nos || count <= 0) {
        return -1;
    }

    ids[0] = '\0';
    for (int i = 0; i < count; i++) {
        char id_str[32];
        snprintf(id_str, sizeof(id_str), "%s%u", (i > 0) ? "," : "", charac_nos[i]);
        strcat(ids, id_str);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_dungeon_test WHERE charac_no IN (%s)", ids);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数 */
void CharacDungeonTest_Free(CharacDungeonTest* record) {
    if (record && record->dungeon) {
        free(record->dungeon);
        record->dungeon = NULL;
        record->dungeon_len = 0;
    }
}

void CharacDungeonTest_PrintInfo(const CharacDungeonTest* record) {
    if (!record) {
        printf("CharacDungeonTest: NULL\n");
        return;
    }

    printf("=== CharacDungeonTest Info ===\n");
    printf("Charac No: %u\n", record->charac_no);
    printf("Dungeon Data Length: %u bytes\n", record->dungeon_len);

    if (record->dungeon && record->dungeon_len > 0) {
        printf("Dungeon Data (first 32 bytes hex): ");
        unsigned int display_len = (record->dungeon_len < 32) ? record->dungeon_len : 32;
        for (unsigned int i = 0; i < display_len; i++) {
            printf("%02x ", record->dungeon[i]);
        }
        if (record->dungeon_len > 32) {
            printf("...");
        }
        printf("\n");
    } else {
        printf("Dungeon Data: (empty)\n");
    }
    printf("==============================\n");
}
