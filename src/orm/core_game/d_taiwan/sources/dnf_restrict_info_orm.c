#include "dnf_restrict_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* CRUD 操作实现 */

int DnfRestrictInfo_Add(DBConnectionManager* manager, const DnfRestrictInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_restrict_info "
        "(category, restrict_code, restrict_str, reg_date) "
        "VALUES (%d, %d, '%s', '%s')",
        record->category,
        record->restrict_code,
        record->restrict_str,
        record->reg_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_Get(DBConnectionManager* manager, int category, int restrict_code, DnfRestrictInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date "
        "FROM dnf_restrict_info "
        "WHERE category = %d AND restrict_code = %d",
        category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 解析结果
    record->category = atoi(values[0]);
    record->restrict_code = atoi(values[1]);
    strncpy(record->restrict_str, values[2], sizeof(record->restrict_str) - 1);
    record->restrict_str[sizeof(record->restrict_str) - 1] = '\0';
    strncpy(record->reg_date, values[3], sizeof(record->reg_date) - 1);
    record->reg_date[sizeof(record->reg_date) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_Update(DBConnectionManager* manager, const DnfRestrictInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_restrict_info "
        "SET restrict_str = '%s', reg_date = '%s' "
        "WHERE category = %d AND restrict_code = %d",
        record->restrict_str,
        record->reg_date,
        record->category,
        record->restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_Delete(DBConnectionManager* manager, int category, int restrict_code) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_restrict_info "
        "WHERE category = %d AND restrict_code = %d",
        category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_Exists(DBConnectionManager* manager, int category, int restrict_code) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_restrict_info "
        "WHERE category = %d AND restrict_code = %d LIMIT 1",
        category, restrict_code);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* 业务操作实现 */

int DnfRestrictInfo_GetByCategory(DBConnectionManager* manager, int category, DnfRestrictInfo** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date "
        "FROM dnf_restrict_info "
        "WHERE category = %d "
        "ORDER BY restrict_code",
        category);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfRestrictInfo*)malloc(sizeof(DnfRestrictInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfRestrictInfo* temp = (DnfRestrictInfo*)realloc(*records, sizeof(DnfRestrictInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].category = atoi(values[0]);
        (*records)[idx].restrict_code = atoi(values[1]);
        strncpy((*records)[idx].restrict_str, values[2], sizeof((*records)[idx].restrict_str) - 1);
        (*records)[idx].restrict_str[sizeof((*records)[idx].restrict_str) - 1] = '\0';
        strncpy((*records)[idx].reg_date, values[3], sizeof((*records)[idx].reg_date) - 1);
        (*records)[idx].reg_date[sizeof((*records)[idx].reg_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_GetAll(DBConnectionManager* manager, DnfRestrictInfo** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date "
        "FROM dnf_restrict_info "
        "ORDER BY category, restrict_code");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfRestrictInfo*)malloc(sizeof(DnfRestrictInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfRestrictInfo* temp = (DnfRestrictInfo*)realloc(*records, sizeof(DnfRestrictInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].category = atoi(values[0]);
        (*records)[idx].restrict_code = atoi(values[1]);
        strncpy((*records)[idx].restrict_str, values[2], sizeof((*records)[idx].restrict_str) - 1);
        (*records)[idx].restrict_str[sizeof((*records)[idx].restrict_str) - 1] = '\0';
        strncpy((*records)[idx].reg_date, values[3], sizeof((*records)[idx].reg_date) - 1);
        (*records)[idx].reg_date[sizeof((*records)[idx].reg_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfRestrictInfo_SearchByString(DBConnectionManager* manager, const char* pattern, DnfRestrictInfo** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !pattern || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT category, restrict_code, restrict_str, reg_date "
        "FROM dnf_restrict_info "
        "WHERE restrict_str LIKE '%%%s%%' "
        "ORDER BY category, restrict_code",
        pattern);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfRestrictInfo*)malloc(sizeof(DnfRestrictInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfRestrictInfo* temp = (DnfRestrictInfo*)realloc(*records, sizeof(DnfRestrictInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].category = atoi(values[0]);
        (*records)[idx].restrict_code = atoi(values[1]);
        strncpy((*records)[idx].restrict_str, values[2], sizeof((*records)[idx].restrict_str) - 1);
        (*records)[idx].restrict_str[sizeof((*records)[idx].restrict_str) - 1] = '\0';
        strncpy((*records)[idx].reg_date, values[3], sizeof((*records)[idx].reg_date) - 1);
        (*records)[idx].reg_date[sizeof((*records)[idx].reg_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数实现 */

void DnfRestrictInfo_PrintInfo(const DnfRestrictInfo* record) {
    if (!record) {
        return;
    }

    printf("=== DNF Restrict Info ===\n");
    printf("Category:      %d\n", record->category);
    printf("Restrict Code: %d\n", record->restrict_code);
    printf("Restrict Str:  %s\n", record->restrict_str);
    printf("Reg Date:      %s\n", record->reg_date);
    printf("========================\n");
}

void DnfRestrictInfo_FreeRecords(DnfRestrictInfo* records) {
    if (records) {
        free(records);
    }
}
