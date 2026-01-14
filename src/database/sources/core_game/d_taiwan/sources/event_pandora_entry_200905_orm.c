#include "event_pandora_entry_200905_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventPandoraEntry200905_Add(DBConnectionManager* manager, const EventPandoraEntry200905* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_pandora_entry_200905 "
        "(m_id, occ_date, server_id, charac_no) "
        "VALUES (%u, '%s', %u, %u)",
        entry->m_id, entry->occ_date, entry->server_id, entry->charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                 unsigned char server_id, EventPandoraEntry200905* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !occ_date || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no "
        "FROM event_pandora_entry_200905 "
        "WHERE m_id = %u AND occ_date = '%s' AND server_id = %u",
        m_id, occ_date, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventPandoraEntry200905));
    entry->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(entry->occ_date, row[1], sizeof(entry->occ_date) - 1);
    entry->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    entry->charac_no = row[3] ? (unsigned int)atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_Update(DBConnectionManager* manager, const EventPandoraEntry200905* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_pandora_entry_200905 "
        "SET charac_no = %u "
        "WHERE m_id = %u AND occ_date = '%s' AND server_id = %u",
        entry->charac_no,
        entry->m_id, entry->occ_date, entry->server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                    unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_date) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_pandora_entry_200905 "
        "WHERE m_id = %u AND occ_date = '%s' AND server_id = %u",
        m_id, occ_date, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                    unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager || !occ_date) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_pandora_entry_200905 "
        "WHERE m_id = %u AND occ_date = '%s' AND server_id = %u",
        m_id, occ_date, server_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0 ? 1 : 0;
}

int EventPandoraEntry200905_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                         EventPandoraEntry200905* entries,
                                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no "
        "FROM event_pandora_entry_200905 "
        "WHERE m_id = %u "
        "ORDER BY occ_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventPandoraEntry200905));
        entries[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_date, row[1], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? (unsigned int)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                       EventPandoraEntry200905* entries,
                                       int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !occ_date || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no "
        "FROM event_pandora_entry_200905 "
        "WHERE occ_date = '%s' "
        "ORDER BY m_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventPandoraEntry200905));
        entries[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_date, row[1], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? (unsigned int)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_GetByCharac(DBConnectionManager* manager, unsigned char server_id,
                                         unsigned int charac_no, EventPandoraEntry200905* entries,
                                         int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no "
        "FROM event_pandora_entry_200905 "
        "WHERE server_id = %u AND charac_no = %u "
        "ORDER BY occ_date DESC",
        server_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventPandoraEntry200905));
        entries[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        if (row[1]) strncpy(entries[count].occ_date, row[1], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? (unsigned int)atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventPandoraEntry200905_DeleteByMember(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_pandora_entry_200905 WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventPandoraEntry200905_PrintInfo(const EventPandoraEntry200905* entry) {
    if (!entry) {
        printf("EventPandoraEntry200905: NULL\n");
        return;
    }

    printf("EventPandoraEntry200905:\n");
    printf("  m_id: %u\n", entry->m_id);
    printf("  occ_date: %s\n", entry->occ_date);
    printf("  server_id: %u\n", entry->server_id);
    printf("  charac_no: %u\n", entry->charac_no);
}
