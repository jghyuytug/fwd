#include "event_skill2025_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventSkill2025Entry_Add(DBConnectionManager* manager, const EventSkill2025Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_skill2025_entry (m_id, server_id, charac_no, occ_time) "
        "VALUES (%d, %u, %d, '%s')",
        entry->m_id, entry->server_id, entry->charac_no, entry->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_Get(DBConnectionManager* manager, int m_id, EventSkill2025Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, occ_time "
        "FROM event_skill2025_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventSkill2025Entry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    entry->server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
    entry->charac_no = row[2] ? atoi(row[2]) : 0;
    if (row[3]) strncpy(entry->occ_time, row[3], sizeof(entry->occ_time) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_Update(DBConnectionManager* manager, const EventSkill2025Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_skill2025_entry SET server_id = %u, charac_no = %d, occ_time = '%s' "
        "WHERE m_id = %d",
        entry->server_id, entry->charac_no, entry->occ_time, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_skill2025_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_skill2025_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventSkill2025Entry_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                     EventSkill2025Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, occ_time "
        "FROM event_skill2025_entry WHERE server_id = %u ORDER BY occ_time DESC, m_id",
        server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventSkill2025Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        entries[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].occ_time, row[3], sizeof(entries[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_GetRecent(DBConnectionManager* manager, int limit,
                                   EventSkill2025Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;
    int actual_limit = (limit < max_count) ? limit : max_count;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, occ_time "
        "FROM event_skill2025_entry ORDER BY occ_time DESC LIMIT %d",
        actual_limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventSkill2025Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        entries[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].occ_time, row[3], sizeof(entries[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventSkill2025Entry_GetAll(DBConnectionManager* manager,
                                EventSkill2025Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, server_id, charac_no, occ_time "
        "FROM event_skill2025_entry ORDER BY m_id LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventSkill2025Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].server_id = row[1] ? (unsigned char)atoi(row[1]) : 0;
        entries[count].charac_no = row[2] ? atoi(row[2]) : 0;
        if (row[3]) strncpy(entries[count].occ_time, row[3], sizeof(entries[count].occ_time) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventSkill2025Entry_PrintInfo(const EventSkill2025Entry* entry) {
    if (!entry) {
        printf("EventSkill2025Entry: NULL\n");
        return;
    }

    printf("EventSkill2025Entry:\n");
    printf("  m_id      = %d\n", entry->m_id);
    printf("  server_id = %u\n", entry->server_id);
    printf("  charac_no = %d\n", entry->charac_no);
    printf("  occ_time  = %s\n", entry->occ_time);
}
