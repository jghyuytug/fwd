#include "dnf_event_address_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* CRUD 操作实现 */

int DnfEventAddress_Add(DBConnectionManager* manager, const DnfEventAddress* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO dnf_event_address "
        "(event_id, m_id, occ_date, zipcode, address, phone_no) "
        "VALUES (%d, %d, '%s', '%s', '%s', '%s')",
        record->event_id,
        record->m_id,
        record->occ_date,
        record->zipcode,
        record->address,
        record->phone_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_Get(DBConnectionManager* manager, int event_id, int m_id, DnfEventAddress* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    // 解析结果
    record->event_id = atoi(values[0]);
    record->m_id = atoi(values[1]);
    strncpy(record->occ_date, values[2], sizeof(record->occ_date) - 1);
    record->occ_date[sizeof(record->occ_date) - 1] = '\0';
    strncpy(record->zipcode, values[3], sizeof(record->zipcode) - 1);
    record->zipcode[sizeof(record->zipcode) - 1] = '\0';
    strncpy(record->address, values[4], sizeof(record->address) - 1);
    record->address[sizeof(record->address) - 1] = '\0';
    strncpy(record->phone_no, values[5], sizeof(record->phone_no) - 1);
    record->phone_no[sizeof(record->phone_no) - 1] = '\0';

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_Update(DBConnectionManager* manager, const DnfEventAddress* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_event_address "
        "SET occ_date = '%s', zipcode = '%s', address = '%s', phone_no = '%s' "
        "WHERE event_id = %d AND m_id = %d",
        record->occ_date,
        record->zipcode,
        record->address,
        record->phone_no,
        record->event_id,
        record->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_Delete(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_address "
        "WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_Exists(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_event_address "
        "WHERE event_id = %d AND m_id = %d LIMIT 1",
        event_id, m_id);

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

int DnfEventAddress_GetByEventId(DBConnectionManager* manager, int event_id, DnfEventAddress** records, int* count) {
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
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "WHERE event_id = %d "
        "ORDER BY m_id",
        event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventAddress*)malloc(sizeof(DnfEventAddress) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventAddress* temp = (DnfEventAddress*)realloc(*records, sizeof(DnfEventAddress) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        strncpy((*records)[idx].occ_date, values[2], sizeof((*records)[idx].occ_date) - 1);
        (*records)[idx].occ_date[sizeof((*records)[idx].occ_date) - 1] = '\0';
        strncpy((*records)[idx].zipcode, values[3], sizeof((*records)[idx].zipcode) - 1);
        (*records)[idx].zipcode[sizeof((*records)[idx].zipcode) - 1] = '\0';
        strncpy((*records)[idx].address, values[4], sizeof((*records)[idx].address) - 1);
        (*records)[idx].address[sizeof((*records)[idx].address) - 1] = '\0';
        strncpy((*records)[idx].phone_no, values[5], sizeof((*records)[idx].phone_no) - 1);
        (*records)[idx].phone_no[sizeof((*records)[idx].phone_no) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_GetByMemberId(DBConnectionManager* manager, int m_id, DnfEventAddress** records, int* count) {
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
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "WHERE m_id = %d "
        "ORDER BY event_id",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventAddress*)malloc(sizeof(DnfEventAddress) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventAddress* temp = (DnfEventAddress*)realloc(*records, sizeof(DnfEventAddress) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        strncpy((*records)[idx].occ_date, values[2], sizeof((*records)[idx].occ_date) - 1);
        (*records)[idx].occ_date[sizeof((*records)[idx].occ_date) - 1] = '\0';
        strncpy((*records)[idx].zipcode, values[3], sizeof((*records)[idx].zipcode) - 1);
        (*records)[idx].zipcode[sizeof((*records)[idx].zipcode) - 1] = '\0';
        strncpy((*records)[idx].address, values[4], sizeof((*records)[idx].address) - 1);
        (*records)[idx].address[sizeof((*records)[idx].address) - 1] = '\0';
        strncpy((*records)[idx].phone_no, values[5], sizeof((*records)[idx].phone_no) - 1);
        (*records)[idx].phone_no[sizeof((*records)[idx].phone_no) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_GetByZipcode(DBConnectionManager* manager, const char* zipcode, DnfEventAddress** records, int* count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int capacity = 10;
    int idx = 0;

    if (!manager || !zipcode || !records || !count) {
        return -1;
    }

    *records = NULL;
    *count = 0;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "WHERE zipcode = '%s' "
        "ORDER BY event_id, m_id",
        zipcode);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventAddress*)malloc(sizeof(DnfEventAddress) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventAddress* temp = (DnfEventAddress*)realloc(*records, sizeof(DnfEventAddress) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        strncpy((*records)[idx].occ_date, values[2], sizeof((*records)[idx].occ_date) - 1);
        (*records)[idx].occ_date[sizeof((*records)[idx].occ_date) - 1] = '\0';
        strncpy((*records)[idx].zipcode, values[3], sizeof((*records)[idx].zipcode) - 1);
        (*records)[idx].zipcode[sizeof((*records)[idx].zipcode) - 1] = '\0';
        strncpy((*records)[idx].address, values[4], sizeof((*records)[idx].address) - 1);
        (*records)[idx].address[sizeof((*records)[idx].address) - 1] = '\0';
        strncpy((*records)[idx].phone_no, values[5], sizeof((*records)[idx].phone_no) - 1);
        (*records)[idx].phone_no[sizeof((*records)[idx].phone_no) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, DnfEventAddress** records, int* count) {
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
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "WHERE occ_date BETWEEN '%s' AND '%s' "
        "ORDER BY occ_date, event_id, m_id",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventAddress*)malloc(sizeof(DnfEventAddress) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventAddress* temp = (DnfEventAddress*)realloc(*records, sizeof(DnfEventAddress) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        strncpy((*records)[idx].occ_date, values[2], sizeof((*records)[idx].occ_date) - 1);
        (*records)[idx].occ_date[sizeof((*records)[idx].occ_date) - 1] = '\0';
        strncpy((*records)[idx].zipcode, values[3], sizeof((*records)[idx].zipcode) - 1);
        (*records)[idx].zipcode[sizeof((*records)[idx].zipcode) - 1] = '\0';
        strncpy((*records)[idx].address, values[4], sizeof((*records)[idx].address) - 1);
        (*records)[idx].address[sizeof((*records)[idx].address) - 1] = '\0';
        strncpy((*records)[idx].phone_no, values[5], sizeof((*records)[idx].phone_no) - 1);
        (*records)[idx].phone_no[sizeof((*records)[idx].phone_no) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventAddress_GetAll(DBConnectionManager* manager, DnfEventAddress** records, int* count) {
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
        "SELECT event_id, m_id, occ_date, zipcode, address, phone_no "
        "FROM dnf_event_address "
        "ORDER BY event_id, m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    *records = (DnfEventAddress*)malloc(sizeof(DnfEventAddress) * capacity);
    if (!*records) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (DBQueryResult_FetchRow(&result, values) > 0) {
        if (idx >= capacity) {
            capacity *= 2;
            DnfEventAddress* temp = (DnfEventAddress*)realloc(*records, sizeof(DnfEventAddress) * capacity);
            if (!temp) {
                free(*records);
                *records = NULL;
                DBQueryResult_Free(&result);
                return -1;
            }
            *records = temp;
        }

        (*records)[idx].event_id = atoi(values[0]);
        (*records)[idx].m_id = atoi(values[1]);
        strncpy((*records)[idx].occ_date, values[2], sizeof((*records)[idx].occ_date) - 1);
        (*records)[idx].occ_date[sizeof((*records)[idx].occ_date) - 1] = '\0';
        strncpy((*records)[idx].zipcode, values[3], sizeof((*records)[idx].zipcode) - 1);
        (*records)[idx].zipcode[sizeof((*records)[idx].zipcode) - 1] = '\0';
        strncpy((*records)[idx].address, values[4], sizeof((*records)[idx].address) - 1);
        (*records)[idx].address[sizeof((*records)[idx].address) - 1] = '\0';
        strncpy((*records)[idx].phone_no, values[5], sizeof((*records)[idx].phone_no) - 1);
        (*records)[idx].phone_no[sizeof((*records)[idx].phone_no) - 1] = '\0';

        idx++;
    }

    *count = idx;
    DBQueryResult_Free(&result);
    return 0;
}

/* 工具函数实现 */

void DnfEventAddress_PrintInfo(const DnfEventAddress* record) {
    if (!record) {
        return;
    }

    printf("=== DNF Event Address ===\n");
    printf("Event ID:   %d\n", record->event_id);
    printf("Member ID:  %d\n", record->m_id);
    printf("Occ Date:   %s\n", record->occ_date);
    printf("Zipcode:    %s\n", record->zipcode);
    printf("Address:    %s\n", record->address);
    printf("Phone No:   %s\n", record->phone_no);
    printf("=========================\n");
}

void DnfEventAddress_FreeRecords(DnfEventAddress* records) {
    if (records) {
        free(records);
    }
}
