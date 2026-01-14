#include "dnf_event_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int DnfEventEntry_Add(DBConnectionManager* manager, const DnfEventEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    if (entry->occ_date[0] && entry->obtain_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_event_entry (event_id, m_id, occ_date, server_id, charac_no, obtain_date) "
            "VALUES (%d, %d, '%s', %u, %d, '%s')",
            entry->event_id, entry->m_id, entry->occ_date, entry->server_id,
            entry->charac_no, entry->obtain_date);
    } else if (entry->occ_date[0]) {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_event_entry (event_id, m_id, occ_date, server_id, charac_no, obtain_date) "
            "VALUES (%d, %d, '%s', %u, %d, NOW())",
            entry->event_id, entry->m_id, entry->occ_date, entry->server_id,
            entry->charac_no);
    } else {
        snprintf(query, sizeof(query),
            "INSERT INTO dnf_event_entry (event_id, m_id, occ_date, server_id, charac_no, obtain_date) "
            "VALUES (%d, %d, NOW(), %u, %d, NOW())",
            entry->event_id, entry->m_id, entry->server_id, entry->charac_no);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_Get(DBConnectionManager* manager, int event_id, int m_id,
                       DnfEventEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(DnfEventEntry));
    entry->event_id = row[0] ? atoi(row[0]) : 0;
    entry->m_id = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(entry->occ_date, row[2], sizeof(entry->occ_date) - 1);
    entry->server_id = row[3] ? (unsigned char)atoi(row[3]) : 0;
    entry->charac_no = row[4] ? atoi(row[4]) : 0;
    if (row[5]) strncpy(entry->obtain_date, row[5], sizeof(entry->obtain_date) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_Update(DBConnectionManager* manager, const DnfEventEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    if (entry->occ_date[0] && entry->obtain_date[0]) {
        snprintf(query, sizeof(query),
            "UPDATE dnf_event_entry SET occ_date = '%s', server_id = %u, "
            "charac_no = %d, obtain_date = '%s' WHERE event_id = %d AND m_id = %d",
            entry->occ_date, entry->server_id, entry->charac_no,
            entry->obtain_date, entry->event_id, entry->m_id);
    } else {
        snprintf(query, sizeof(query),
            "UPDATE dnf_event_entry SET occ_date = NOW(), server_id = %u, "
            "charac_no = %d, obtain_date = NOW() WHERE event_id = %d AND m_id = %d",
            entry->server_id, entry->charac_no, entry->event_id, entry->m_id);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_Delete(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_event_entry WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_Exists(DBConnectionManager* manager, int event_id, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_event_entry WHERE event_id = %d AND m_id = %d",
        event_id, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int DnfEventEntry_GetByMemberId(DBConnectionManager* manager, int m_id,
                                 DnfEventEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry WHERE m_id = %d ORDER BY event_id", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(DnfEventEntry));
        entries[count].event_id = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(entries[count].occ_date, row[2], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[3] ? (unsigned char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(entries[count].obtain_date, row[5], sizeof(entries[count].obtain_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_GetByEventId(DBConnectionManager* manager, int event_id,
                                DnfEventEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry WHERE event_id = %d ORDER BY m_id", event_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(DnfEventEntry));
        entries[count].event_id = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(entries[count].occ_date, row[2], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[3] ? (unsigned char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(entries[count].obtain_date, row[5], sizeof(entries[count].obtain_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_GetByCharacter(DBConnectionManager* manager, int charac_no,
                                  DnfEventEntry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry WHERE charac_no = %d ORDER BY event_id", charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(DnfEventEntry));
        entries[count].event_id = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(entries[count].occ_date, row[2], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[3] ? (unsigned char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(entries[count].obtain_date, row[5], sizeof(entries[count].obtain_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int DnfEventEntry_GetAll(DBConnectionManager* manager, DnfEventEntry* entries,
                          int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT event_id, m_id, occ_date, server_id, charac_no, obtain_date "
        "FROM dnf_event_entry ORDER BY event_id, m_id LIMIT %d", max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_LOGIN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(DnfEventEntry));
        entries[count].event_id = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(entries[count].occ_date, row[2], sizeof(entries[count].occ_date) - 1);
        entries[count].server_id = row[3] ? (unsigned char)atoi(row[3]) : 0;
        entries[count].charac_no = row[4] ? atoi(row[4]) : 0;
        if (row[5]) strncpy(entries[count].obtain_date, row[5], sizeof(entries[count].obtain_date) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void DnfEventEntry_PrintInfo(const DnfEventEntry* entry) {
    if (!entry) return;

    printf("Event Entry Info:\n");
    printf("  Event ID: %d\n", entry->event_id);
    printf("  Member ID: %d\n", entry->m_id);
    printf("  Occurrence Date: %s\n", entry->occ_date);
    printf("  Server ID: %u\n", entry->server_id);
    printf("  Character No: %d\n", entry->charac_no);
    printf("  Obtain Date: %s\n", entry->obtain_date);
}
