#include "dnf_event_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* CRUD 操作实现 */

int DnfEventInfo_Add(DBConnectionManager* manager, const DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_event_info "
        "(event_id, event_name, event_explain, apply_type, start_date, end_date) "
        "VALUES (%d, '%s', '%s', %d, '%s', '%s')",
        record->event_id,
        record->event_name,
        record->event_explain,
        (int)record->apply_type,
        record->start_date,
        record->end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_Get(DBConnectionManager* manager, int event_id, DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE event_id = %d",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 解析结果
    record->event_id = atoi(values[0]);
    strncpy(record->event_name, values[1], sizeof(record->event_name) - 1);
    record->event_name[sizeof(record->event_name) - 1] = '\0';
    strncpy(record->event_explain, values[2], sizeof(record->event_explain) - 1);
    record->event_explain[sizeof(record->event_explain) - 1] = '\0';
    record->apply_type = (char)atoi(values[3]);
    strncpy(record->start_date, values[4], sizeof(record->start_date) - 1);
    record->start_date[sizeof(record->start_date) - 1] = '\0';
    strncpy(record->end_date, values[5], sizeof(record->end_date) - 1);
    record->end_date[sizeof(record->end_date) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_GetByName(DBConnectionManager* manager, const char* event_name, DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !event_name || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE event_name = '%s'",
        event_name);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 解析结果
    record->event_id = atoi(values[0]);
    strncpy(record->event_name, values[1], sizeof(record->event_name) - 1);
    record->event_name[sizeof(record->event_name) - 1] = '\0';
    strncpy(record->event_explain, values[2], sizeof(record->event_explain) - 1);
    record->event_explain[sizeof(record->event_explain) - 1] = '\0';
    record->apply_type = (char)atoi(values[3]);
    strncpy(record->start_date, values[4], sizeof(record->start_date) - 1);
    record->start_date[sizeof(record->start_date) - 1] = '\0';
    strncpy(record->end_date, values[5], sizeof(record->end_date) - 1);
    record->end_date[sizeof(record->end_date) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_Update(DBConnectionManager* manager, const DnfEventInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_event_info "
        "SET event_name = '%s', event_explain = '%s', apply_type = %d, "
        "start_date = '%s', end_date = '%s' "
        "WHERE event_id = %d",
        record->event_name,
        record->event_explain,
        (int)record->apply_type,
        record->start_date,
        record->end_date,
        record->event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_Delete(DBConnectionManager* manager, int event_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_info WHERE event_id = %d",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_Exists(DBConnectionManager* manager, int event_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_info WHERE event_id = %d LIMIT 1",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int DnfEventInfo_NameExists(DBConnectionManager* manager, const char* event_name) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager || !event_name) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_info WHERE event_name = '%s' LIMIT 1",
        event_name);

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

int DnfEventInfo_GetByApplyType(DBConnectionManager* manager, char apply_type, DnfEventInfo** records, int* count) {
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
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE apply_type = %d "
        "ORDER BY event_id",
        (int)apply_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventInfo*)malloc(sizeof(DnfEventInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventInfo* temp = (DnfEventInfo*)realloc(*records, sizeof(DnfEventInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        strncpy((*records)[idx].event_name, values[1], sizeof((*records)[idx].event_name) - 1);
        (*records)[idx].event_name[sizeof((*records)[idx].event_name) - 1] = '\0';
        strncpy((*records)[idx].event_explain, values[2], sizeof((*records)[idx].event_explain) - 1);
        (*records)[idx].event_explain[sizeof((*records)[idx].event_explain) - 1] = '\0';
        (*records)[idx].apply_type = (char)atoi(values[3]);
        strncpy((*records)[idx].start_date, values[4], sizeof((*records)[idx].start_date) - 1);
        (*records)[idx].start_date[sizeof((*records)[idx].start_date) - 1] = '\0';
        strncpy((*records)[idx].end_date, values[5], sizeof((*records)[idx].end_date) - 1);
        (*records)[idx].end_date[sizeof((*records)[idx].end_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_GetActiveEvents(DBConnectionManager* manager, const char* date, DnfEventInfo** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !date || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE start_date <= '%s' AND end_date >= '%s' "
        "ORDER BY event_id",
        date, date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventInfo*)malloc(sizeof(DnfEventInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventInfo* temp = (DnfEventInfo*)realloc(*records, sizeof(DnfEventInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        strncpy((*records)[idx].event_name, values[1], sizeof((*records)[idx].event_name) - 1);
        (*records)[idx].event_name[sizeof((*records)[idx].event_name) - 1] = '\0';
        strncpy((*records)[idx].event_explain, values[2], sizeof((*records)[idx].event_explain) - 1);
        (*records)[idx].event_explain[sizeof((*records)[idx].event_explain) - 1] = '\0';
        (*records)[idx].apply_type = (char)atoi(values[3]);
        strncpy((*records)[idx].start_date, values[4], sizeof((*records)[idx].start_date) - 1);
        (*records)[idx].start_date[sizeof((*records)[idx].start_date) - 1] = '\0';
        strncpy((*records)[idx].end_date, values[5], sizeof((*records)[idx].end_date) - 1);
        (*records)[idx].end_date[sizeof((*records)[idx].end_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, DnfEventInfo** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !start_date || !end_date || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE start_date >= '%s' AND end_date <= '%s' "
        "ORDER BY start_date, event_id",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventInfo*)malloc(sizeof(DnfEventInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventInfo* temp = (DnfEventInfo*)realloc(*records, sizeof(DnfEventInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        strncpy((*records)[idx].event_name, values[1], sizeof((*records)[idx].event_name) - 1);
        (*records)[idx].event_name[sizeof((*records)[idx].event_name) - 1] = '\0';
        strncpy((*records)[idx].event_explain, values[2], sizeof((*records)[idx].event_explain) - 1);
        (*records)[idx].event_explain[sizeof((*records)[idx].event_explain) - 1] = '\0';
        (*records)[idx].apply_type = (char)atoi(values[3]);
        strncpy((*records)[idx].start_date, values[4], sizeof((*records)[idx].start_date) - 1);
        (*records)[idx].start_date[sizeof((*records)[idx].start_date) - 1] = '\0';
        strncpy((*records)[idx].end_date, values[5], sizeof((*records)[idx].end_date) - 1);
        (*records)[idx].end_date[sizeof((*records)[idx].end_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_SearchByName(DBConnectionManager* manager, const char* pattern, DnfEventInfo** records, int* count) {
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
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "WHERE event_name LIKE '%%%s%%' "
        "ORDER BY event_id",
        pattern);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventInfo*)malloc(sizeof(DnfEventInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventInfo* temp = (DnfEventInfo*)realloc(*records, sizeof(DnfEventInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        strncpy((*records)[idx].event_name, values[1], sizeof((*records)[idx].event_name) - 1);
        (*records)[idx].event_name[sizeof((*records)[idx].event_name) - 1] = '\0';
        strncpy((*records)[idx].event_explain, values[2], sizeof((*records)[idx].event_explain) - 1);
        (*records)[idx].event_explain[sizeof((*records)[idx].event_explain) - 1] = '\0';
        (*records)[idx].apply_type = (char)atoi(values[3]);
        strncpy((*records)[idx].start_date, values[4], sizeof((*records)[idx].start_date) - 1);
        (*records)[idx].start_date[sizeof((*records)[idx].start_date) - 1] = '\0';
        strncpy((*records)[idx].end_date, values[5], sizeof((*records)[idx].end_date) - 1);
        (*records)[idx].end_date[sizeof((*records)[idx].end_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_GetAll(DBConnectionManager* manager, DnfEventInfo** records, int* count) {
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
        "SELECT event_id, event_name, event_explain, apply_type, start_date, end_date "
        "FROM dnf_event_info "
        "ORDER BY event_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventInfo*)malloc(sizeof(DnfEventInfo) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventInfo* temp = (DnfEventInfo*)realloc(*records, sizeof(DnfEventInfo) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        strncpy((*records)[idx].event_name, values[1], sizeof((*records)[idx].event_name) - 1);
        (*records)[idx].event_name[sizeof((*records)[idx].event_name) - 1] = '\0';
        strncpy((*records)[idx].event_explain, values[2], sizeof((*records)[idx].event_explain) - 1);
        (*records)[idx].event_explain[sizeof((*records)[idx].event_explain) - 1] = '\0';
        (*records)[idx].apply_type = (char)atoi(values[3]);
        strncpy((*records)[idx].start_date, values[4], sizeof((*records)[idx].start_date) - 1);
        (*records)[idx].start_date[sizeof((*records)[idx].start_date) - 1] = '\0';
        strncpy((*records)[idx].end_date, values[5], sizeof((*records)[idx].end_date) - 1);
        (*records)[idx].end_date[sizeof((*records)[idx].end_date) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventInfo_IsExpired(DBConnectionManager* manager, int event_id, const char* current_date) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int is_expired = 0;

    if (!manager || !current_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_info "
        "WHERE event_id = %d AND end_date < '%s' LIMIT 1",
        event_id, current_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        is_expired = 1;
    }

    DBQueryResult_Free(&result);
    return is_expired;
}

/* 工具函数实现 */

void DnfEventInfo_PrintInfo(const DnfEventInfo* record) {
    if (!record) {
        return;
    }

    printf("=== DNF Event Info ===\n");
    printf("Event ID:     %d\n", record->event_id);
    printf("Event Name:   %s\n", record->event_name);
    printf("Explain:      %s\n", record->event_explain);
    printf("Apply Type:   %d\n", (int)record->apply_type);
    printf("Start Date:   %s\n", record->start_date);
    printf("End Date:     %s\n", record->end_date);
    printf("=====================\n");
}

void DnfEventInfo_FreeRecords(DnfEventInfo* records) {
    if (records) {
        free(records);
    }
}
