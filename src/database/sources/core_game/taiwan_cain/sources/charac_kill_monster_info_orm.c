#include "charac_kill_monster_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 200000

/* 辅助函数：将二进制数据转换为十六进制字符串 */
static char* bin_to_hex(const unsigned char* data, unsigned int len) {
    char* hex;
    unsigned int i;

    if (!data || len == 0) {
        return NULL;
    }

    hex = (char*)malloc(len * 2 + 1);
    if (!hex) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02X", data[i]);
    }
    hex[len * 2] = '\0';

    return hex;
}

/* 辅助函数：将十六进制字符串转换为二进制数据 */
static unsigned char* hex_to_bin(const char* hex, unsigned int* out_len) {
    unsigned char* bin;
    unsigned int len;
    unsigned int i;

    if (!hex || !out_len) {
        return NULL;
    }

    len = strlen(hex) / 2;
    bin = (unsigned char*)malloc(len);
    if (!bin) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        sscanf(hex + i * 2, "%2hhX", &bin[i]);
    }

    *out_len = len;
    return bin;
}

/* ==================== CRUD 操作实现 ==================== */

int CharacKillMonsterInfo_Add(DBConnectionManager* manager, const CharacKillMonsterInfo* record) {
    char* query;
    char* boss_value;
    char* named_value;
    char* apc_value;
    DBQueryResult result;
    char* boss_hex = NULL;
    char* named_hex = NULL;
    char* apc_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    boss_value = (char*)malloc(MAX_QUERY_LEN / 3);
    named_value = (char*)malloc(MAX_QUERY_LEN / 3);
    apc_value = (char*)malloc(MAX_QUERY_LEN / 3);

    if (!query || !boss_value || !named_value || !apc_value) {
        free(query);
        free(boss_value);
        free(named_value);
        free(apc_value);
        return -1;
    }

    boss_hex = bin_to_hex(record->boss_info, record->boss_info_len);
    named_hex = bin_to_hex(record->named_info, record->named_info_len);
    apc_hex = bin_to_hex(record->apc_boss_info, record->apc_boss_info_len);

    if (boss_hex) {
        snprintf(boss_value, MAX_QUERY_LEN / 3, "0x%s", boss_hex);
    } else {
        strcpy(boss_value, "NULL");
    }

    if (named_hex) {
        snprintf(named_value, MAX_QUERY_LEN / 3, "0x%s", named_hex);
    } else {
        strcpy(named_value, "NULL");
    }

    if (apc_hex) {
        snprintf(apc_value, MAX_QUERY_LEN / 3, "0x%s", apc_hex);
    } else {
        strcpy(apc_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "REPLACE INTO charac_kill_monster_info "
        "(charac_no, boss_info, named_info, apc_boss_info) "
        "VALUES (%u, %s, %s, %s)",
        record->charac_no,
        boss_value, named_value, apc_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(boss_value);
    free(named_value);
    free(apc_value);
    free(boss_hex);
    free(named_hex);
    free(apc_hex);

    return ret;
}

int CharacKillMonsterInfo_Get(DBConnectionManager* manager, unsigned int charac_no, CharacKillMonsterInfo* record) {
    char query[1024];
    DBQueryResult result;
    char* values[4];

    if (!manager || !record) {
        return -1;
    }

    memset(record, 0, sizeof(CharacKillMonsterInfo));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(boss_info), HEX(named_info), HEX(apc_boss_info) "
        "FROM charac_kill_monster_info WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);

        if (values[1] && strlen(values[1]) > 0) {
            record->boss_info = hex_to_bin(values[1], &record->boss_info_len);
        }

        if (values[2] && strlen(values[2]) > 0) {
            record->named_info = hex_to_bin(values[2], &record->named_info_len);
        }

        if (values[3] && strlen(values[3]) > 0) {
            record->apc_boss_info = hex_to_bin(values[3], &record->apc_boss_info_len);
        }

        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacKillMonsterInfo_Update(DBConnectionManager* manager, const CharacKillMonsterInfo* record) {
    char* query;
    char* boss_value;
    char* named_value;
    char* apc_value;
    DBQueryResult result;
    char* boss_hex = NULL;
    char* named_hex = NULL;
    char* apc_hex = NULL;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    boss_value = (char*)malloc(MAX_QUERY_LEN / 3);
    named_value = (char*)malloc(MAX_QUERY_LEN / 3);
    apc_value = (char*)malloc(MAX_QUERY_LEN / 3);

    if (!query || !boss_value || !named_value || !apc_value) {
        free(query);
        free(boss_value);
        free(named_value);
        free(apc_value);
        return -1;
    }

    boss_hex = bin_to_hex(record->boss_info, record->boss_info_len);
    named_hex = bin_to_hex(record->named_info, record->named_info_len);
    apc_hex = bin_to_hex(record->apc_boss_info, record->apc_boss_info_len);

    if (boss_hex) {
        snprintf(boss_value, MAX_QUERY_LEN / 3, "0x%s", boss_hex);
    } else {
        strcpy(boss_value, "NULL");
    }

    if (named_hex) {
        snprintf(named_value, MAX_QUERY_LEN / 3, "0x%s", named_hex);
    } else {
        strcpy(named_value, "NULL");
    }

    if (apc_hex) {
        snprintf(apc_value, MAX_QUERY_LEN / 3, "0x%s", apc_hex);
    } else {
        strcpy(apc_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_kill_monster_info SET "
        "boss_info = %s, named_info = %s, apc_boss_info = %s "
        "WHERE charac_no = %u",
        boss_value, named_value, apc_value,
        record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(boss_value);
    free(named_value);
    free(apc_value);
    free(boss_hex);
    free(named_hex);
    free(apc_hex);

    return ret;
}

int CharacKillMonsterInfo_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[256];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_kill_monster_info WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacKillMonsterInfo_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[256];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_kill_monster_info WHERE charac_no = %u", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/* ==================== 专项更新操作实现 ==================== */

int CharacKillMonsterInfo_UpdateBossInfo(DBConnectionManager* manager, unsigned int charac_no,
                                          const unsigned char* boss_info, unsigned int boss_info_len) {
    char* query;
    char* boss_value;
    DBQueryResult result;
    char* boss_hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    boss_value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !boss_value) {
        free(query);
        free(boss_value);
        return -1;
    }

    boss_hex = bin_to_hex(boss_info, boss_info_len);

    if (boss_hex) {
        snprintf(boss_value, MAX_QUERY_LEN / 2, "0x%s", boss_hex);
    } else {
        strcpy(boss_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_kill_monster_info SET boss_info = %s WHERE charac_no = %u",
        boss_value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(boss_value);
    free(boss_hex);

    return ret;
}

int CharacKillMonsterInfo_UpdateNamedInfo(DBConnectionManager* manager, unsigned int charac_no,
                                           const unsigned char* named_info, unsigned int named_info_len) {
    char* query;
    char* named_value;
    DBQueryResult result;
    char* named_hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    named_value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !named_value) {
        free(query);
        free(named_value);
        return -1;
    }

    named_hex = bin_to_hex(named_info, named_info_len);

    if (named_hex) {
        snprintf(named_value, MAX_QUERY_LEN / 2, "0x%s", named_hex);
    } else {
        strcpy(named_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_kill_monster_info SET named_info = %s WHERE charac_no = %u",
        named_value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(named_value);
    free(named_hex);

    return ret;
}

int CharacKillMonsterInfo_UpdateApcBossInfo(DBConnectionManager* manager, unsigned int charac_no,
                                             const unsigned char* apc_boss_info, unsigned int apc_boss_info_len) {
    char* query;
    char* apc_value;
    DBQueryResult result;
    char* apc_hex = NULL;
    int ret = -1;

    if (!manager) {
        return -1;
    }

    query = (char*)malloc(MAX_QUERY_LEN);
    apc_value = (char*)malloc(MAX_QUERY_LEN / 2);

    if (!query || !apc_value) {
        free(query);
        free(apc_value);
        return -1;
    }

    apc_hex = bin_to_hex(apc_boss_info, apc_boss_info_len);

    if (apc_hex) {
        snprintf(apc_value, MAX_QUERY_LEN / 2, "0x%s", apc_hex);
    } else {
        strcpy(apc_value, "NULL");
    }

    snprintf(query, MAX_QUERY_LEN,
        "UPDATE charac_kill_monster_info SET apc_boss_info = %s WHERE charac_no = %u",
        apc_value, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) >= 0) {
        DBQueryResult_Free(&result);
        ret = 0;
    }

    free(query);
    free(apc_value);
    free(apc_hex);

    return ret;
}

/* ==================== 工具函数实现 ==================== */

void CharacKillMonsterInfo_Free(CharacKillMonsterInfo* record) {
    if (!record) {
        return;
    }

    if (record->boss_info) {
        free(record->boss_info);
        record->boss_info = NULL;
        record->boss_info_len = 0;
    }

    if (record->named_info) {
        free(record->named_info);
        record->named_info = NULL;
        record->named_info_len = 0;
    }

    if (record->apc_boss_info) {
        free(record->apc_boss_info);
        record->apc_boss_info = NULL;
        record->apc_boss_info_len = 0;
    }
}

void CharacKillMonsterInfo_PrintInfo(const CharacKillMonsterInfo* record) {
    if (!record) {
        printf("CharacKillMonsterInfo: NULL\n");
        return;
    }

    printf("CharacKillMonsterInfo {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  boss_info: %u bytes\n", record->boss_info_len);
    printf("  named_info: %u bytes\n", record->named_info_len);
    printf("  apc_boss_info: %u bytes\n", record->apc_boss_info_len);
    printf("}\n");
}
