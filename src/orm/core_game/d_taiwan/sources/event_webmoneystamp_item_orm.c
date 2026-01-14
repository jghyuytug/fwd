#include "event_webmoneystamp_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int EventWebmoneystampItem_Add(DBConnectionManager* manager, const EventWebmoneystampItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO event_webmoneystamp_item "
        "(m_id, occ_time, server_id, charac_no, item_no, item_check) "
        "VALUES (%d, %d, %u, %u, %u, %u)",
        item->m_id, item->occ_time, item->server_id, item->charac_no,
        item->item_no, item->item_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_Get(DBConnectionManager* manager, int m_id, int occ_time,
                                EventWebmoneystampItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_no, item_check "
        "FROM event_webmoneystamp_item "
        "WHERE m_id = %d AND occ_time = %d",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(item, 0, sizeof(EventWebmoneystampItem));
    item->m_id = row[0] ? atoi(row[0]) : 0;
    item->occ_time = row[1] ? atoi(row[1]) : 0;
    item->server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
    item->charac_no = row[3] ? (unsigned char)atoi(row[3]) : 0;
    item->item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
    item->item_check = row[5] ? (unsigned int)atoi(row[5]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_Update(DBConnectionManager* manager, const EventWebmoneystampItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "UPDATE event_webmoneystamp_item "
        "SET server_id = %u, charac_no = %u, item_no = %u, item_check = %u "
        "WHERE m_id = %d AND occ_time = %d",
        item->server_id, item->charac_no, item->item_no, item->item_check,
        item->m_id, item->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_Delete(DBConnectionManager* manager, int m_id, int occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_webmoneystamp_item WHERE m_id = %d AND occ_time = %d",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_Exists(DBConnectionManager* manager, int m_id, int occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM event_webmoneystamp_item WHERE m_id = %d AND occ_time = %d",
        m_id, occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return exists > 0 ? 1 : 0;
}

int EventWebmoneystampItem_GetByMember(DBConnectionManager* manager, int m_id,
                                        EventWebmoneystampItem* items,
                                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !items || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_no, item_check "
        "FROM event_webmoneystamp_item "
        "WHERE m_id = %d "
        "ORDER BY occ_time DESC",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&items[count], 0, sizeof(EventWebmoneystampItem));
        items[count].m_id = row[0] ? atoi(row[0]) : 0;
        items[count].occ_time = row[1] ? atoi(row[1]) : 0;
        items[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        items[count].charac_no = row[3] ? (unsigned char)atoi(row[3]) : 0;
        items[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        items[count].item_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_GetByCharac(DBConnectionManager* manager, unsigned char server_id,
                                        unsigned char charac_no, EventWebmoneystampItem* items,
                                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !items || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_no, item_check "
        "FROM event_webmoneystamp_item "
        "WHERE server_id = %u AND charac_no = %u "
        "ORDER BY occ_time DESC",
        server_id, charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&items[count], 0, sizeof(EventWebmoneystampItem));
        items[count].m_id = row[0] ? atoi(row[0]) : 0;
        items[count].occ_time = row[1] ? atoi(row[1]) : 0;
        items[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        items[count].charac_no = row[3] ? (unsigned char)atoi(row[3]) : 0;
        items[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        items[count].item_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_GetByItemNo(DBConnectionManager* manager, unsigned int item_no,
                                        EventWebmoneystampItem* items,
                                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[6];
    int count = 0;

    if (!manager || !items || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, occ_time, server_id, charac_no, item_no, item_check "
        "FROM event_webmoneystamp_item "
        "WHERE item_no = %u "
        "ORDER BY occ_time DESC",
        item_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&items[count], 0, sizeof(EventWebmoneystampItem));
        items[count].m_id = row[0] ? atoi(row[0]) : 0;
        items[count].occ_time = row[1] ? atoi(row[1]) : 0;
        items[count].server_id = row[2] ? (unsigned char)atoi(row[2]) : 0;
        items[count].charac_no = row[3] ? (unsigned char)atoi(row[3]) : 0;
        items[count].item_no = row[4] ? (unsigned int)atoi(row[4]) : 0;
        items[count].item_check = row[5] ? (unsigned int)atoi(row[5]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int EventWebmoneystampItem_DeleteByMember(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM event_webmoneystamp_item WHERE m_id = %d",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

void EventWebmoneystampItem_PrintInfo(const EventWebmoneystampItem* item) {
    if (!item) {
        printf("EventWebmoneystampItem: NULL\n");
        return;
    }

    printf("EventWebmoneystampItem:\n");
    printf("  m_id: %d\n", item->m_id);
    printf("  occ_time: %d\n", item->occ_time);
    printf("  server_id: %u\n", item->server_id);
    printf("  charac_no: %u\n", item->charac_no);
    printf("  item_no: %u\n", item->item_no);
    printf("  item_check: %u\n", item->item_check);
}
