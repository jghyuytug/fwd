#include "event_newmember0709_entry_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventNewmember0709Entry_Add(DBConnectionManager* manager, const EventNewmember0709Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_newmember0709_entry "
        "(m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check) "
        "VALUES (%d, %u, %u, %d, %u, %u, %u, %u)",
        entry->m_id, entry->occ_date, entry->server_id, entry->charac_no,
        entry->item1_no, entry->item1_check, entry->item2_no, entry->item2_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_Get(DBConnectionManager* manager, int m_id, EventNewmember0709Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check "
        "FROM event_newmember0709_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(entry, 0, sizeof(EventNewmember0709Entry));
    entry->m_id = row[0] ? atoi(row[0]) : 0;
    entry->occ_date = row[1] ? (unsigned int)atoi(row[1]) : 0;
    entry->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    entry->charac_no = row[3] ? atoi(row[3]) : 0;
    entry->item1_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
    entry->item1_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
    entry->item2_no = row[6] ? (unsigned int)atoi(row[6]) : 0;
    entry->item2_check = row[7] ? (unsigned int)atoi(row[7]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_Update(DBConnectionManager* manager, const EventNewmember0709Entry* entry) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !entry) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_newmember0709_entry SET "
        "occ_date = %u, server_id = %u, charac_no = %d, "
        "item1_no = %u, item1_check = %u, item2_no = %u, item2_check = %u "
        "WHERE m_id = %d",
        entry->occ_date, entry->server_id, entry->charac_no,
        entry->item1_no, entry->item1_check, entry->item2_no, entry->item2_check,
        entry->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_Delete(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_newmember0709_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_Exists(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_newmember0709_entry WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

int EventNewmember0709Entry_GetByDate(DBConnectionManager* manager, unsigned int occ_date,
                                       EventNewmember0709Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check "
        "FROM event_newmember0709_entry WHERE occ_date = %u ORDER BY m_id",
        occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventNewmember0709Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item1_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item1_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].item2_no = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].item2_check = row[7] ? (unsigned int)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_GetByDateRange(DBConnectionManager* manager,
                                            unsigned int start_date, unsigned int end_date,
                                            EventNewmember0709Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check "
        "FROM event_newmember0709_entry WHERE occ_date BETWEEN %u AND %u ORDER BY occ_date DESC, m_id",
        start_date, end_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventNewmember0709Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item1_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item1_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].item2_no = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].item2_check = row[7] ? (unsigned int)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventNewmember0709Entry_GetAll(DBConnectionManager* manager,
                                    EventNewmember0709Entry* entries, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[8];
    int count = 0;

    if (!manager || !entries || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_date, server_id, charac_no, item1_no, item1_check, item2_no, item2_check "
        "FROM event_newmember0709_entry ORDER BY occ_date DESC, m_id LIMIT %d",
        max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&entries[count], 0, sizeof(EventNewmember0709Entry));
        entries[count].m_id = row[0] ? atoi(row[0]) : 0;
        entries[count].occ_date = row[1] ? (unsigned int)atoi(row[1]) : 0;
        entries[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        entries[count].charac_no = row[3] ? atoi(row[3]) : 0;
        entries[count].item1_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        entries[count].item1_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        entries[count].item2_no = row[6] ? (unsigned int)atoi(row[6]) : 0;
        entries[count].item2_check = row[7] ? (unsigned int)atoi(row[7]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

void EventNewmember0709Entry_PrintInfo(const EventNewmember0709Entry* entry) {
    if (!entry) {
        printf("EventNewmember0709Entry: NULL\n");
        return;
    }

    printf("EventNewmember0709Entry:\n");
    printf("  m_id         = %d\n", entry->m_id);
    printf("  occ_date     = %u\n", entry->occ_date);
    printf("  server_id    = %u\n", entry->server_id);
    printf("  charac_no    = %d\n", entry->charac_no);
    printf("  item1_no     = %u\n", entry->item1_no);
    printf("  item1_check  = %u\n", entry->item1_check);
    printf("  item2_no     = %u\n", entry->item2_no);
    printf("  item2_check  = %u\n", entry->item2_check);
}
