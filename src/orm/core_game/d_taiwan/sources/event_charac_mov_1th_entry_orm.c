#include "event_charac_mov_1th_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventCharacMov1thEntry_Add(DBConnectionManager* manager, const EventCharacMov1thEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_charac_mov_1th_entry (m_id, occ_time, it_no, item_check) "
        "VALUES (%d, %d, %d, %d)",
        entry->m_id, entry->occ_time, entry->it_no, entry->item_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1thEntry_Get(DBConnectionManager* manager, int m_id, EventCharacMov1thEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, it_no, item_check FROM event_charac_mov_1th_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventCharacMov1thEntry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    entry->occ_time = row[1] ? atoi(row[1]) : 0;
    entry->it_no = row[2] ? atoi(row[2]) : 0;
    entry->item_check = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1thEntry_Update(DBConnectionManager* manager, const EventCharacMov1thEntry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_charac_mov_1th_entry SET occ_time = %d, it_no = %d, item_check = %d "
        "WHERE m_id = %d",
        entry->occ_time, entry->it_no, entry->item_check, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1thEntry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_charac_mov_1th_entry WHERE m_id = %d", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1thEntry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT 1 FROM event_charac_mov_1th_entry WHERE m_id = %d LIMIT 1", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventCharacMov1thEntry_GetAll(DBConnectionManager* manager, EventCharacMov1thEntry* entries,
                                   int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, it_no, item_check FROM event_charac_mov_1th_entry ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventCharacMov1thEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_time = row[1] ? atoi(row[1]) : 0;
        entries[count].it_no = row[2] ? atoi(row[2]) : 0;
        entries[count].item_check = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventCharacMov1thEntry_GetByItemNo(DBConnectionManager* manager, int it_no,
                                        EventCharacMov1thEntry* entries,
                                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[4];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, it_no, item_check FROM event_charac_mov_1th_entry "
        "WHERE it_no = %d ORDER BY m_id", it_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventCharacMov1thEntry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_time = row[1] ? atoi(row[1]) : 0;
        entries[count].it_no = row[2] ? atoi(row[2]) : 0;
        entries[count].item_check = row[3] ? atoi(row[3]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventCharacMov1thEntry_PrintInfo(const EventCharacMov1thEntry* entry) {
    if (!entry) return;

    printf("EventCharacMov1thEntry:\n");
    printf("  m_id: %d\n", entry->m_id);
    printf("  occ_time: %d\n", entry->occ_time);
    printf("  it_no: %d\n", entry->it_no);
    printf("  item_check: %d\n", entry->item_check);
}