#include "dnf_event_prize_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* CRUD 操作实现 */

int DnfEventPrize_Add(DBConnectionManager* manager, const DnfEventPrize* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_event_prize (prize_id, m_id, check_time) VALUES (%d, %d, %d)",
        record->prize_id, record->m_id, record->check_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_Get(DBConnectionManager* manager, int prize_id, int m_id, DnfEventPrize* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT prize_id, m_id, check_time FROM dnf_event_prize WHERE prize_id = %d AND m_id = %d",
        prize_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    record->prize_id = atoi(values[0]);
    record->m_id = atoi(values[1]);
    record->check_time = atoi(values[2]);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_Update(DBConnectionManager* manager, const DnfEventPrize* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_event_prize SET check_time = %d WHERE prize_id = %d AND m_id = %d",
        record->check_time, record->prize_id, record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_Delete(DBConnectionManager* manager, int prize_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_prize WHERE prize_id = %d AND m_id = %d",
        prize_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_Exists(DBConnectionManager* manager, int prize_id, int m_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_prize WHERE prize_id = %d AND m_id = %d LIMIT 1",
        prize_id, m_id);

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

int DnfEventPrize_GetByPrizeId(DBConnectionManager* manager, int prize_id, DnfEventPrize** records, int* count) {
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
        "SELECT prize_id, m_id, check_time FROM dnf_event_prize WHERE prize_id = %d ORDER BY m_id",
        prize_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventPrize*)malloc(sizeof(DnfEventPrize) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventPrize* temp = (DnfEventPrize*)realloc(*records, sizeof(DnfEventPrize) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].prize_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        (*records)[idx].check_time = atoi(values[2]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_GetByMemberId(DBConnectionManager* manager, int m_id, DnfEventPrize** records, int* count) {
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
        "SELECT prize_id, m_id, check_time FROM dnf_event_prize WHERE m_id = %d ORDER BY prize_id",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventPrize*)malloc(sizeof(DnfEventPrize) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventPrize* temp = (DnfEventPrize*)realloc(*records, sizeof(DnfEventPrize) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].prize_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        (*records)[idx].check_time = atoi(values[2]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_GetByTimeRange(DBConnectionManager* manager, int start_time, int end_time, DnfEventPrize** records, int* count) {
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
        "SELECT prize_id, m_id, check_time FROM dnf_event_prize WHERE check_time BETWEEN %d AND %d ORDER BY check_time",
        start_time, end_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventPrize*)malloc(sizeof(DnfEventPrize) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventPrize* temp = (DnfEventPrize*)realloc(*records, sizeof(DnfEventPrize) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].prize_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        (*records)[idx].check_time = atoi(values[2]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_GetAll(DBConnectionManager* manager, DnfEventPrize** records, int* count) {
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
        "SELECT prize_id, m_id, check_time FROM dnf_event_prize ORDER BY prize_id, m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventPrize*)malloc(sizeof(DnfEventPrize) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventPrize* temp = (DnfEventPrize*)realloc(*records, sizeof(DnfEventPrize) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].prize_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        (*records)[idx].check_time = atoi(values[2]);
        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventPrize_CountByPrizeId(DBConnectionManager* manager, int prize_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int prize_count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_prize WHERE prize_id = %d",
        prize_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        prize_count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return prize_count;
}

int DnfEventPrize_CountByMemberId(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int prize_count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_prize WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        prize_count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return prize_count;
}

/* 工具函数实现 */

void DnfEventPrize_PrintInfo(const DnfEventPrize* record) {
    if (!record) {
        return;
    }

    printf("=== DNF Event Prize ===\n");
    printf("Prize ID:    %d\n", record->prize_id);
    printf("Member ID:   %d\n", record->m_id);
    printf("Check Time:  %d\n", record->check_time);
    printf("=======================\n");
}

void DnfEventPrize_FreeRecords(DnfEventPrize* records) {
    if (records) {
        free(records);
    }
}
