#include "event_goldcard_entry2_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventGoldcardEntry2_Add(DBConnectionManager* manager, const EventGoldcardEntry2* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_entry2 (occ_date, m_id, server_id, charac_no, item_no, item_check) "
        "VALUES (%d, %d, %u, %d, %u, %d)",
        entry->occ_date, entry->m_id, entry->server_id,
        entry->charac_no, entry->item_no, entry->item_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_Get(DBConnectionManager* manager, int occ_date, int m_id, EventGoldcardEntry2* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check "
        "FROM event_goldcard_entry2 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventGoldcardEntry2));
    entry->occ_date = row[0] ? atoi(row[0]) : 0;
    entry->m_id = row[1] ? atoi(row[1]) : 0;
    entry->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    entry->charac_no = row[3] ? atoi(row[3]) : 0;
    entry->item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
    entry->item_check = row[5] ? atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_Update(DBConnectionManager* manager, const EventGoldcardEntry2* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_entry2 SET server_id = %u, charac_no = %d, "
        "item_no = %u, item_check = %d WHERE occ_date = %d AND m_id = %d",
        entry->server_id, entry->charac_no, entry->item_no, entry->item_check,
        entry->occ_date, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_Delete(DBConnectionManager* manager, int occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_goldcard_entry2 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_Exists(DBConnectionManager* manager, int occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_goldcard_entry2 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventGoldcardEntry2_GetByMId(DBConnectionManager* manager, int m_id,
                                  EventGoldcardEntry2* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check "
        "FROM event_goldcard_entry2 WHERE m_id = %d ORDER BY occ_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry2));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item_check = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_GetByDate(DBConnectionManager* manager, int occ_date,
                                   EventGoldcardEntry2* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check "
        "FROM event_goldcard_entry2 WHERE occ_date = %d ORDER BY m_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry2));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item_check = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry2_GetAll(DBConnectionManager* manager,
                                EventGoldcardEntry2* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, server_id, charac_no, item_no, item_check "
        "FROM event_goldcard_entry2 ORDER BY occ_date DESC, m_id LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry2));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item_check = row[5] ? atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventGoldcardEntry2_PrintInfo(const EventGoldcardEntry2* entry) {
    if (!entry) {
        printf("EventGoldcardEntry2: NULL\n");
        return;
    }

    printf("EventGoldcardEntry2:\n");
    printf("  occ_date    = %d\n", entry->occ_date);
    printf("  m_id        = %d\n", entry->m_id);
    printf("  server_id   = %u\n", entry->server_id);
    printf("  charac_no   = %d\n", entry->charac_no);
    printf("  item_no     = %u\n", entry->item_no);
    printf("  item_check  = %d\n", entry->item_check);
}
