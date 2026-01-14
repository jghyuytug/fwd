#include "limit_npc_item_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int LimitNpcItem_Add(DBConnectionManager* manager, const LimitNpcItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO limit_npc_item (item_index, max_count, sell_count) VALUES (%u, %u, %u)",
        record->item_index, record->max_count, record->sell_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitNpcItem_Get(DBConnectionManager* manager, unsigned int item_index, LimitNpcItem* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(LimitNpcItem));

    snprintf(query, sizeof(query),
        "SELECT item_index, max_count, sell_count FROM limit_npc_item WHERE item_index = %u",
        item_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->item_index = (unsigned int)strtoul(values[0], NULL, 10);
        record->max_count = (unsigned int)strtoul(values[1], NULL, 10);
        record->sell_count = (unsigned int)strtoul(values[2], NULL, 10);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int LimitNpcItem_Update(DBConnectionManager* manager, const LimitNpcItem* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE limit_npc_item SET max_count = %u, sell_count = %u WHERE item_index = %u",
        record->max_count, record->sell_count, record->item_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitNpcItem_Delete(DBConnectionManager* manager, unsigned int item_index) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM limit_npc_item WHERE item_index = %u",
        item_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitNpcItem_Exists(DBConnectionManager* manager, unsigned int item_index) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limit_npc_item WHERE item_index = %u",
        item_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int LimitNpcItem_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM limit_npc_item");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int LimitNpcItem_GetAll(DBConnectionManager* manager, LimitNpcItem* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT item_index, max_count, sell_count FROM limit_npc_item ORDER BY item_index LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].item_index = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].max_count = (unsigned int)strtoul(values[1], NULL, 10);
        records[count].sell_count = (unsigned int)strtoul(values[2], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int LimitNpcItem_GetSoldOut(DBConnectionManager* manager, LimitNpcItem* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT item_index, max_count, sell_count FROM limit_npc_item WHERE sell_count >= max_count ORDER BY item_index LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].item_index = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].max_count = (unsigned int)strtoul(values[1], NULL, 10);
        records[count].sell_count = (unsigned int)strtoul(values[2], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int LimitNpcItem_IncreaseSellCount(DBConnectionManager* manager, unsigned int item_index, unsigned int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || count == 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE limit_npc_item SET sell_count = sell_count + %u WHERE item_index = %u",
        count, item_index);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void LimitNpcItem_PrintInfo(const LimitNpcItem* record) {
    if (!record) {
        printf("LimitNpcItem: NULL\n");
        return;
    }

    printf("=== Limit NPC Item ===\n");
    printf("  item_index: %u\n", record->item_index);
    printf("  max_count: %u\n", record->max_count);
    printf("  sell_count: %u\n", record->sell_count);
    printf("  remaining: %u\n", (record->sell_count < record->max_count) ?
           (record->max_count - record->sell_count) : 0);
    printf("  status: %s\n", (record->sell_count >= record->max_count) ? "SOLD OUT" : "Available");
    printf("======================\n");
}
