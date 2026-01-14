#include "event_aradlotto_0809_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELDS 3
#define MAX_QUERY_LEN 4096

/* CRUD operations */

int EventAradlotto0809Entry_Add(DBConnectionManager* manager, const EventAradlotto0809Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "REPLACE INTO event_aradlotto_0809_entry (m_id, occ_date, lotto_num) "
        "VALUES (%d, %d, '%s')",
        entry->m_id, entry->occ_date, entry->lotto_num);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradlotto0809Entry_Get(DBConnectionManager* manager, int m_id, EventAradlotto0809Entry* entry) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, lotto_num FROM event_aradlotto_0809_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventAradlotto0809Entry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    entry->occ_date = row[1] ? atoi(row[1]) : 0;
    if (row[2]) {
        strncpy(entry->lotto_num, row[2], sizeof(entry->lotto_num) - 1);
        entry->lotto_num[sizeof(entry->lotto_num) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradlotto0809Entry_Update(DBConnectionManager* manager, const EventAradlotto0809Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_aradlotto_0809_entry SET occ_date = %d, lotto_num = '%s' WHERE m_id = %d",
        entry->occ_date, entry->lotto_num, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradlotto0809Entry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_aradlotto_0809_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int EventAradlotto0809Entry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    char* row[1];
    DBQueryResult result;
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_aradlotto_0809_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) > 0 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Business operations */

int EventAradlotto0809Entry_GetByLottoNum(DBConnectionManager* manager, const char* lotto_num,
                                           EventAradlotto0809Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !lotto_num || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, lotto_num FROM event_aradlotto_0809_entry WHERE lotto_num = '%s' ORDER BY m_id LIMIT %d",
        lotto_num, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        *actual_count = 0;
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventAradlotto0809Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(entries[count].lotto_num, row[2], sizeof(entries[count].lotto_num) - 1);
            entries[count].lotto_num[sizeof(entries[count].lotto_num) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventAradlotto0809Entry_CountByLottoNum(DBConnectionManager* manager, const char* lotto_num) {
    char query[MAX_QUERY_LEN];
    char* row[1];
    DBQueryResult result;
    int count = 0;

    if (!manager || !lotto_num) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_aradlotto_0809_entry WHERE lotto_num = '%s'", lotto_num);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        return 0;
    }

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return count;
}

int EventAradlotto0809Entry_GetByDateRange(DBConnectionManager* manager, int start_date, int end_date,
                                            EventAradlotto0809Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    char* row[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, lotto_num FROM event_aradlotto_0809_entry "
        "WHERE occ_date >= %d AND occ_date <= %d ORDER BY occ_date DESC LIMIT %d",
        start_date, end_date, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        *actual_count = 0;
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventAradlotto0809Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? atoi(row[1]) : 0;
        if (row[2]) {
            strncpy(entries[count].lotto_num, row[2], sizeof(entries[count].lotto_num) - 1);
            entries[count].lotto_num[sizeof(entries[count].lotto_num) - 1] = '\0';
        }
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventAradlotto0809Entry_PrintInfo(const EventAradlotto0809Entry* entry) {
    if (!entry) return;

    printf("EventAradlotto0809Entry:\n");
    printf("  Member ID: %d\n", entry->m_id);
    printf("  Occur Date: %d\n", entry->occ_date);
    printf("  Lotto Number: %s\n", entry->lotto_num);
}
