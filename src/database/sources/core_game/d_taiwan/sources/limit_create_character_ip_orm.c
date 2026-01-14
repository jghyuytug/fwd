#include "limit_create_character_ip_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_QUERY_LEN 4096

/* 辅助函数：IP字符串转整数 */
unsigned int LimitCreateCharacterIP_StrToInt(const char* ip_str) {
    struct in_addr addr;
    if (inet_aton(ip_str, &addr) == 0) {
        return 0;
    }
    return ntohl(addr.s_addr);
}

/* 辅助函数：IP整数转字符串 */
void LimitCreateCharacterIP_IntToStr(unsigned int ip, char* ip_str, size_t len) {
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    const char* str = inet_ntoa(addr);
    if (str && len > 0) {
        strncpy(ip_str, str, len - 1);
        ip_str[len - 1] = '\0';
    }
}

/* CRUD 操作实现 */
int LimitCreateCharacterIP_Add(DBConnectionManager* manager, const LimitCreateCharacterIP* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO limit_create_character_ip (ip, ip_str, last_access_time, count, last_access_mid) "
        "VALUES (%u, '%s', '%s', %u, %u)",
        record->ip,
        record->ip_str,
        record->last_access_time,
        record->count,
        record->last_access_mid);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_Get(DBConnectionManager* manager, unsigned int ip, LimitCreateCharacterIP* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip WHERE ip = %u",
        ip);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->ip = (unsigned int)strtoul(values[0], NULL, 10);
    strncpy(record->ip_str, values[1] ? values[1] : "", sizeof(record->ip_str) - 1);
    record->ip_str[sizeof(record->ip_str) - 1] = '\0';
    strncpy(record->last_access_time, values[2] ? values[2] : "", sizeof(record->last_access_time) - 1);
    record->last_access_time[sizeof(record->last_access_time) - 1] = '\0';
    record->count = (unsigned int)strtoul(values[3], NULL, 10);
    record->last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_GetByIPStr(DBConnectionManager* manager, const char* ip_str, LimitCreateCharacterIP* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;

    if (!manager || !ip_str || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip WHERE ip_str = '%s'",
        ip_str);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->ip = (unsigned int)strtoul(values[0], NULL, 10);
    strncpy(record->ip_str, values[1] ? values[1] : "", sizeof(record->ip_str) - 1);
    record->ip_str[sizeof(record->ip_str) - 1] = '\0';
    strncpy(record->last_access_time, values[2] ? values[2] : "", sizeof(record->last_access_time) - 1);
    record->last_access_time[sizeof(record->last_access_time) - 1] = '\0';
    record->count = (unsigned int)strtoul(values[3], NULL, 10);
    record->last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_Update(DBConnectionManager* manager, const LimitCreateCharacterIP* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET ip_str = '%s', last_access_time = '%s', "
        "count = %u, last_access_mid = %u WHERE ip = %u",
        record->ip_str,
        record->last_access_time,
        record->count,
        record->last_access_mid,
        record->ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_Delete(DBConnectionManager* manager, unsigned int ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limit_create_character_ip WHERE ip = %u",
        ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_Exists(DBConnectionManager* manager, unsigned int ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int exists = 0;

    if (!manager) {
        return 0;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limit_create_character_ip WHERE ip = %u",
        ip);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* 专项操作 */
int LimitCreateCharacterIP_IncrementCount(DBConnectionManager* manager, unsigned int ip, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET count = count + 1, "
        "last_access_time = NOW(), last_access_mid = %u WHERE ip = %u",
        m_id, ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_ResetCount(DBConnectionManager* manager, unsigned int ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET count = 0, last_access_time = NOW() WHERE ip = %u",
        ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_UpdateAccessTime(DBConnectionManager* manager, unsigned int ip) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET last_access_time = NOW() WHERE ip = %u",
        ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_UpdateAccessMid(DBConnectionManager* manager, unsigned int ip, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET last_access_mid = %u, last_access_time = NOW() WHERE ip = %u",
        m_id, ip);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 查询操作 */
int LimitCreateCharacterIP_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int ret;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM limit_create_character_ip");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int LimitCreateCharacterIP_GetAll(DBConnectionManager* manager, LimitCreateCharacterIP** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip ORDER BY ip");

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

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

    *records = (LimitCreateCharacterIP*)malloc(sizeof(LimitCreateCharacterIP) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].ip = (unsigned int)strtoul(values[0], NULL, 10);
        strncpy((*records)[i].ip_str, values[1] ? values[1] : "", sizeof((*records)[i].ip_str) - 1);
        (*records)[i].ip_str[sizeof((*records)[i].ip_str) - 1] = '\0';
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        (*records)[i].count = (unsigned int)strtoul(values[3], NULL, 10);
        (*records)[i].last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_GetByCountRange(DBConnectionManager* manager, unsigned int min_count, unsigned int max_count,
                                           LimitCreateCharacterIP** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip WHERE count BETWEEN %u AND %u ORDER BY count DESC",
        min_count, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

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

    *records = (LimitCreateCharacterIP*)malloc(sizeof(LimitCreateCharacterIP) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].ip = (unsigned int)strtoul(values[0], NULL, 10);
        strncpy((*records)[i].ip_str, values[1] ? values[1] : "", sizeof((*records)[i].ip_str) - 1);
        (*records)[i].ip_str[sizeof((*records)[i].ip_str) - 1] = '\0';
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        (*records)[i].count = (unsigned int)strtoul(values[3], NULL, 10);
        (*records)[i].last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_GetRecentAccess(DBConnectionManager* manager, int days,
                                           LimitCreateCharacterIP** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip WHERE last_access_time >= DATE_SUB(NOW(), INTERVAL %d DAY) "
        "ORDER BY last_access_time DESC",
        days);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

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

    *records = (LimitCreateCharacterIP*)malloc(sizeof(LimitCreateCharacterIP) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].ip = (unsigned int)strtoul(values[0], NULL, 10);
        strncpy((*records)[i].ip_str, values[1] ? values[1] : "", sizeof((*records)[i].ip_str) - 1);
        (*records)[i].ip_str[sizeof((*records)[i].ip_str) - 1] = '\0';
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        (*records)[i].count = (unsigned int)strtoul(values[3], NULL, 10);
        (*records)[i].last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_GetByMid(DBConnectionManager* manager, unsigned int m_id,
                                    LimitCreateCharacterIP** records, int* count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[5];
    int ret;
    int i;

    if (!manager || !records || !count) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip, ip_str, last_access_time, count, last_access_mid "
        "FROM limit_create_character_ip WHERE last_access_mid = %u "
        "ORDER BY last_access_time DESC",
        m_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

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

    *records = (LimitCreateCharacterIP*)malloc(sizeof(LimitCreateCharacterIP) * (*count));
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    i = 0;
    while (DBQueryResult_FetchRow(&result, values) > 0 && i < *count) {
        (*records)[i].ip = (unsigned int)strtoul(values[0], NULL, 10);
        strncpy((*records)[i].ip_str, values[1] ? values[1] : "", sizeof((*records)[i].ip_str) - 1);
        (*records)[i].ip_str[sizeof((*records)[i].ip_str) - 1] = '\0';
        strncpy((*records)[i].last_access_time, values[2] ? values[2] : "", sizeof((*records)[i].last_access_time) - 1);
        (*records)[i].last_access_time[sizeof((*records)[i].last_access_time) - 1] = '\0';
        (*records)[i].count = (unsigned int)strtoul(values[3], NULL, 10);
        (*records)[i].last_access_mid = (unsigned int)strtoul(values[4], NULL, 10);
        i++;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 批量操作 */
int LimitCreateCharacterIP_Clear(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM limit_create_character_ip");

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_BatchDelete(DBConnectionManager* manager, const unsigned int* ips, int count) {
    char query[MAX_QUERY_LEN];
    char ids[2048];
    DBQueryResult result;

    if (!manager || !ips || count <= 0) {
        return -1;
    }

    ids[0] = '\0';
    for (int i = 0; i < count; i++) {
        char id_str[32];
        snprintf(id_str, sizeof(id_str), "%s%u", (i > 0) ? "," : "", ips[i]);
        strcat(ids, id_str);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limit_create_character_ip WHERE ip IN (%s)", ids);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitCreateCharacterIP_BatchResetCount(DBConnectionManager* manager, const unsigned int* ips, int count) {
    char query[MAX_QUERY_LEN];
    char ids[2048];
    DBQueryResult result;

    if (!manager || !ips || count <= 0) {
        return -1;
    }

    ids[0] = '\0';
    for (int i = 0; i < count; i++) {
        char id_str[32];
        snprintf(id_str, sizeof(id_str), "%s%u", (i > 0) ? "," : "", ips[i]);
        strcat(ids, id_str);
    }

    snprintf(query, sizeof(query),
        "UPDATE limit_create_character_ip SET count = 0, last_access_time = NOW() WHERE ip IN (%s)", ids);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数 */
void LimitCreateCharacterIP_PrintInfo(const LimitCreateCharacterIP* record) {
    if (!record) {
        printf("LimitCreateCharacterIP: NULL\n");
        return;
    }

    printf("=== LimitCreateCharacterIP Info ===\n");
    printf("IP (int): %u\n", record->ip);
    printf("IP (str): %s\n", record->ip_str);
    printf("Create Count: %u\n", record->count);
    printf("Last Access Time: %s\n", record->last_access_time);
    printf("Last Access MID: %u\n", record->last_access_mid);
    printf("====================================\n");
}
