#include "event_goldcard_entry1_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventGoldcardEntry1_Add(DBConnectionManager* manager, const EventGoldcardEntry1* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_goldcard_entry1 (occ_date, m_id, item_no) "
        "VALUES (%d, %d, %u)",
        entry->occ_date, entry->m_id, entry->item_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_Get(DBConnectionManager* manager, int occ_date, int m_id, EventGoldcardEntry1* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, item_no "
        "FROM event_goldcard_entry1 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventGoldcardEntry1));
    entry->occ_date = row[0] ? atoi(row[0]) : 0;
    entry->m_id = row[1] ? atoi(row[1]) : 0;
    entry->item_no = row[2] ? (unsigned char)atoi(row[2]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_Update(DBConnectionManager* manager, const EventGoldcardEntry1* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_goldcard_entry1 SET item_no = %u "
        "WHERE occ_date = %d AND m_id = %d",
        entry->item_no, entry->occ_date, entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_Delete(DBConnectionManager* manager, int occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_goldcard_entry1 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_Exists(DBConnectionManager* manager, int occ_date, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_goldcard_entry1 WHERE occ_date = %d AND m_id = %d",
        occ_date, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventGoldcardEntry1_GetByMId(DBConnectionManager* manager, int m_id,
                                  EventGoldcardEntry1* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, item_no "
        "FROM event_goldcard_entry1 WHERE m_id = %d ORDER BY occ_date DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry1));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].item_no = row[2] ? (unsigned char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_GetByDate(DBConnectionManager* manager, int occ_date,
                                   EventGoldcardEntry1* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, item_no "
        "FROM event_goldcard_entry1 WHERE occ_date = %d ORDER BY m_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry1));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].item_no = row[2] ? (unsigned char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventGoldcardEntry1_GetAll(DBConnectionManager* manager,
                                EventGoldcardEntry1* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[3];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT occ_date, m_id, item_no "
        "FROM event_goldcard_entry1 ORDER BY occ_date DESC, m_id LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventGoldcardEntry1));
        entries[count].occ_date = row[0] ? atoi(row[0]) : 0;
        entries[count].m_id = row[1] ? atoi(row[1]) : 0;
        entries[count].item_no = row[2] ? (unsigned char)atoi(row[2]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventGoldcardEntry1_PrintInfo(const EventGoldcardEntry1* entry) {
    if (!entry) {
        printf("EventGoldcardEntry1: NULL\n");
        return;
    }

    printf("EventGoldcardEntry1:\n");
    printf("  occ_date = %d\n", entry->occ_date);
    printf("  m_id     = %d\n", entry->m_id);
    printf("  item_no  = %u\n", entry->item_no);
}
