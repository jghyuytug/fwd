#include "check_pick_up_random_option_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

/* CRUD Operations */

int CheckPickUpRandomOptionItem_Add(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "REPLACE INTO check_pick_up_random_option_item (m_id, check_count) "
        "VALUES (%u, %u)",
        item->m_id, item->check_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_Get(DBConnectionManager* manager, unsigned int m_id, CheckPickUpRandomOptionItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, check_count FROM check_pick_up_random_option_item WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(item, 0, sizeof(CheckPickUpRandomOptionItem));
    item->m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
    item->check_count = row[1] ? (unsigned char)atoi(row[1]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_Update(DBConnectionManager* manager, const CheckPickUpRandomOptionItem* item) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !item) return -1;

    snprintf(query, sizeof(query),
        "UPDATE check_pick_up_random_option_item SET check_count = %u WHERE m_id = %u",
        item->check_count, item->m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_Delete(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "DELETE FROM check_pick_up_random_option_item WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_Exists(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int exists = 0;

    if (!manager) return 0;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM check_pick_up_random_option_item WHERE m_id = %u", m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return 0;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        exists = (row[0] && atoi(row[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* Specialized Operations */

int CheckPickUpRandomOptionItem_IncrementCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE check_pick_up_random_option_item SET check_count = check_count + 1 WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_ResetCount(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE check_pick_up_random_option_item SET check_count = 0 WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_UpdateCount(DBConnectionManager* manager, unsigned int m_id, unsigned char new_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "UPDATE check_pick_up_random_option_item SET check_count = %u WHERE m_id = %u",
        new_count, m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Query Operations */

int CheckPickUpRandomOptionItem_GetAll(DBConnectionManager* manager, CheckPickUpRandomOptionItem* items,
                                        int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !items || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, check_count FROM check_pick_up_random_option_item ORDER BY m_id");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&items[count], 0, sizeof(CheckPickUpRandomOptionItem));
        items[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        items[count].check_count = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_GetByCountRange(DBConnectionManager* manager, unsigned char min_count,
                                                  unsigned char max_count, CheckPickUpRandomOptionItem* items,
                                                  int max_items, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[2];
    int count = 0;

    if (!manager || !items || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT m_id, check_count FROM check_pick_up_random_option_item "
        "WHERE check_count >= %u AND check_count <= %u ORDER BY check_count DESC, m_id",
        min_count, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    while (count < max_items && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&items[count], 0, sizeof(CheckPickUpRandomOptionItem));
        items[count].m_id = row[0] ? (unsigned int)atoi(row[0]) : 0;
        items[count].check_count = row[1] ? (unsigned char)atoi(row[1]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}

int CheckPickUpRandomOptionItem_CountAll(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM check_pick_up_random_option_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

int CheckPickUpRandomOptionItem_CountByCount(DBConnectionManager* manager, unsigned char check_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[1];
    int total_count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM check_pick_up_random_option_item WHERE check_count = %u",
        check_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) > 0) {
        total_count = row[0] ? atoi(row[0]) : 0;
    }

    DBQueryResult_Free(&result);
    return total_count;
}

/* Batch Operations */

int CheckPickUpRandomOptionItem_BatchDelete(DBConnectionManager* manager, const unsigned int* m_ids, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int offset = 0;
    int i;

    if (!manager || !m_ids || count <= 0) return -1;

    offset = snprintf(query, sizeof(query),
        "DELETE FROM check_pick_up_random_option_item WHERE m_id IN (");

    for (i = 0; i < count && offset < (int)sizeof(query) - 50; i++) {
        if (i > 0) {
            offset += snprintf(query + offset, sizeof(query) - offset, ",");
        }
        offset += snprintf(query + offset, sizeof(query) - offset, "%u", m_ids[i]);
    }

    snprintf(query + offset, sizeof(query) - offset, ")");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

/* Utility Functions */

void CheckPickUpRandomOptionItem_PrintInfo(const CheckPickUpRandomOptionItem* item) {
    if (!item) {
        printf("CheckPickUpRandomOptionItem: NULL\n");
        return;
    }

    printf("=== Check Pick Up Random Option Item ===\n");
    printf("Member ID:     %u\n", item->m_id);
    printf("Check Count:   %u\n", item->check_count);
    printf("========================================\n");
}
