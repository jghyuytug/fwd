#include "event_webmoneystamp_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventWebmoneystampEntry_Add(DBConnectionManager* manager, const EventWebmoneystampEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_webmoneystamp_entry "
        "(m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item) "
        "VALUES (%d, '%s', %u, '%s', %d, %d)",
        entry->m_id, entry->occ_time, entry->attend_point,
        entry->last_attend_time, entry->return_flag, entry->entry_item);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_Get(DBConnectionManager* manager, int m_id, EventWebmoneystampEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item "
        "FROM event_webmoneystamp_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventWebmoneystampEntry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    if (row[1]) strncpy(entry->occ_time, row[1], sizeof(entry->occ_time) - 1);
    entry->attend_point = row[2] ? (unsigned short)atoi(row[2]) : 0;
    if (row[3]) strncpy(entry->last_attend_time, row[3], sizeof(entry->last_attend_time) - 1);
    entry->return_flag = row[4] ? (char)atoi(row[4]) : 0;
    entry->entry_item = row[5] ? (char)atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_Update(DBConnectionManager* manager, const EventWebmoneystampEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_webmoneystamp_entry SET "
        "occ_time = '%s', attend_point = %u, last_attend_time = '%s', "
        "return_flag = %d, entry_item = %d "
        "WHERE m_id = %d",
        entry->occ_time, entry->attend_point, entry->last_attend_time,
        entry->return_flag, entry->entry_item, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_webmoneystamp_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_webmoneystamp_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventWebmoneystampEntry_GetByReturnFlag(DBConnectionManager* manager, char return_flag,
                                             EventWebmoneystampEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item "
        "FROM event_webmoneystamp_entry WHERE return_flag = %d ORDER BY m_id",
        return_flag);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventWebmoneystampEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
        entries[count].attend_point = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].last_attend_time, row[3], sizeof(entries[count].last_attend_time) - 1);
        entries[count].return_flag = row[4] ? (char)atoi(row[4]) : 0;
        entries[count].entry_item = row[5] ? (char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_GetByPointRange(DBConnectionManager* manager,
                                              unsigned short min_point, unsigned short max_point,
                                              EventWebmoneystampEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item "
        "FROM event_webmoneystamp_entry WHERE attend_point BETWEEN %u AND %u ORDER BY attend_point DESC, m_id",
        min_point, max_point);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventWebmoneystampEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
        entries[count].attend_point = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].last_attend_time, row[3], sizeof(entries[count].last_attend_time) - 1);
        entries[count].return_flag = row[4] ? (char)atoi(row[4]) : 0;
        entries[count].entry_item = row[5] ? (char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampEntry_GetAll(DBConnectionManager* manager,
                                    EventWebmoneystampEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, attend_point, last_attend_time, return_flag, entry_item "
        "FROM event_webmoneystamp_entry ORDER BY m_id LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventWebmoneystampEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_time, row[1], sizeof(entries[count].occ_time) - 1);
        entries[count].attend_point = row[2] ? (unsigned short)atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].last_attend_time, row[3], sizeof(entries[count].last_attend_time) - 1);
        entries[count].return_flag = row[4] ? (char)atoi(row[4]) : 0;
        entries[count].entry_item = row[5] ? (char)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventWebmoneystampEntry_PrintInfo(const EventWebmoneystampEntry* entry) {
    if (!entry) {
        printf("EventWebmoneystampEntry: NULL\n");
        return;
    }

    printf("EventWebmoneystampEntry:\n");
    printf("  m_id             = %d\n", entry->m_id);
    printf("  occ_time         = %s\n", entry->occ_time);
    printf("  attend_point     = %u\n", entry->attend_point);
    printf("  last_attend_time = %s\n", entry->last_attend_time);
    printf("  return_flag      = %d\n", entry->return_flag);
    printf("  entry_item       = %d\n", entry->entry_item);
}
