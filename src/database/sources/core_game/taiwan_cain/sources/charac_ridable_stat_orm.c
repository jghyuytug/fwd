#include "charac_ridable_stat_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 65536  /* BLOB需要更大的查询缓冲区 */
#define MAX_FIELDS 10

/*******************************************************************************
 * 辅助函数
 ******************************************************************************/

/* 将二进制数据转换为十六进制字符串 */
static void bin_to_hex(const unsigned char* bin, int bin_len, char* hex) {
    int i;
    for (i = 0; i < bin_len; i++) {
        sprintf(hex + (i * 2), "%02x", bin[i]);
    }
    hex[bin_len * 2] = '\0';
}

/* 将十六进制字符串转换为二进制数据 */
static int hex_to_bin(const char* hex, unsigned char* bin, int max_bin_len) {
    int hex_len = strlen(hex);
    int bin_len = hex_len / 2;
    int i;

    if (bin_len > max_bin_len) {
        bin_len = max_bin_len;
    }

    for (i = 0; i < bin_len; i++) {
        sscanf(hex + (i * 2), "%2hhx", &bin[i]);
    }

    return bin_len;
}

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacRidableStat_Add(DBConnectionManager* manager, const CharacRidableStat* record) {
    char query[MAX_QUERY_LEN];
    char hex_cooltime[MAX_COOLTIME_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record || record->cooltime_len <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 将二进制数据转换为十六进制 */
    bin_to_hex(record->cooltime, record->cooltime_len, hex_cooltime);

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_ridable_stat (charac_no, cooltime) "
        "VALUES (%u, UNHEX('%s'))",
        record->charac_no, hex_cooltime);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacRidableStat_Get(DBConnectionManager* manager, unsigned int charac_no,
                          CharacRidableStat* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(CharacRidableStat));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(cooltime) FROM charac_ridable_stat WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            record->cooltime_len = hex_to_bin(values[1], record->cooltime, MAX_COOLTIME_LEN);
        } else {
            record->cooltime_len = 0;
        }
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacRidableStat_Update(DBConnectionManager* manager, const CharacRidableStat* record) {
    char query[MAX_QUERY_LEN];
    char hex_cooltime[MAX_COOLTIME_LEN * 2 + 1];
    DBQueryResult result;

    if (!manager || !record || record->cooltime_len <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 将二进制数据转换为十六进制 */
    bin_to_hex(record->cooltime, record->cooltime_len, hex_cooltime);

    snprintf(query, sizeof(query),
        "UPDATE charac_ridable_stat SET cooltime = UNHEX('%s') WHERE charac_no = %u",
        hex_cooltime, record->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacRidableStat_Delete(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM charac_ridable_stat WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;  /* DELETE幂等操作 */
}

int CharacRidableStat_Exists(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_ridable_stat WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int CharacRidableStat_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_ridable_stat");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacRidableStat_GetAll(DBConnectionManager* manager, CharacRidableStat* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, HEX(cooltime) FROM charac_ridable_stat LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        if (values[1]) {
            records[count].cooltime_len = hex_to_bin(values[1], records[count].cooltime, MAX_COOLTIME_LEN);
        } else {
            records[count].cooltime_len = 0;
        }
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int CharacRidableStat_ClearCooltime(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_ridable_stat SET cooltime = '' WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacRidableStat_BatchDelete(DBConnectionManager* manager,
                                  const unsigned int* charac_nos, int count) {
    char query[MAX_QUERY_LEN];
    char in_clause[MAX_QUERY_LEN];
    DBQueryResult result;
    int i, offset = 0;

    if (!manager || !charac_nos || count <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    /* 构建 IN 子句 */
    for (i = 0; i < count && offset < sizeof(in_clause) - 20; i++) {
        if (i > 0) {
            offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, ",");
        }
        offset += snprintf(in_clause + offset, sizeof(in_clause) - offset, "%u", charac_nos[i]);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_ridable_stat WHERE charac_no IN (%s)",
        in_clause);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

int CharacRidableStat_EnsureExists(DBConnectionManager* manager, unsigned int charac_no) {
    CharacRidableStat record;
    int exists;

    if (!manager) {
        return -1;
    }

    exists = CharacRidableStat_Exists(manager, charac_no);
    if (exists == 1) {
        return 0;  /* 已存在 */
    } else if (exists < 0) {
        return -1;  /* 查询失败 */
    }

    /* 不存在，创建空记录 */
    memset(&record, 0, sizeof(CharacRidableStat));
    record.charac_no = charac_no;
    record.cooltime[0] = 0;
    record.cooltime_len = 1;

    return CharacRidableStat_Add(manager, &record);
}

void CharacRidableStat_PrintInfo(const CharacRidableStat* record) {
    int i;

    if (!record) {
        printf("CharacRidableStat: NULL\n");
        return;
    }

    printf("=== Charac Ridable Stat ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  cooltime_len: %d bytes\n", record->cooltime_len);
    if (record->cooltime_len > 0 && record->cooltime_len <= 64) {
        printf("  cooltime (hex): ");
        for (i = 0; i < record->cooltime_len && i < 64; i++) {
            printf("%02x ", record->cooltime[i]);
        }
        printf("\n");
    } else if (record->cooltime_len > 64) {
        printf("  cooltime (hex, first 64 bytes): ");
        for (i = 0; i < 64; i++) {
            printf("%02x ", record->cooltime[i]);
        }
        printf("...\n");
    }
    printf("===========================\n");
}
