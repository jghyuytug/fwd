#include "item_select_std_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int ItemSelectStd_Add(DBConnectionManager* manager, const ItemSelectStd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO item_select_std (item_grade, top, bottom, weight) VALUES (%d, %d, %d, %d)",
        record->item_grade, record->top, record->bottom, record->weight);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemSelectStd_Get(DBConnectionManager* manager, int item_grade, ItemSelectStd* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(ItemSelectStd));

    snprintf(query, sizeof(query),
        "SELECT item_grade, top, bottom, weight FROM item_select_std WHERE item_grade = %d",
        item_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->item_grade = atoi(values[0]);
        record->top = atoi(values[1]);
        record->bottom = atoi(values[2]);
        record->weight = atoi(values[3]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int ItemSelectStd_Update(DBConnectionManager* manager, const ItemSelectStd* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_select_std SET top = %d, bottom = %d, weight = %d WHERE item_grade = %d",
        record->top, record->bottom, record->weight, record->item_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemSelectStd_Delete(DBConnectionManager* manager, int item_grade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM item_select_std WHERE item_grade = %d",
        item_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemSelectStd_Exists(DBConnectionManager* manager, int item_grade) {
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
        "SELECT COUNT(*) FROM item_select_std WHERE item_grade = %d",
        item_grade);

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

int ItemSelectStd_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM item_select_std");

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

int ItemSelectStd_GetAll(DBConnectionManager* manager, ItemSelectStd* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT item_grade, top, bottom, weight FROM item_select_std ORDER BY item_grade LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].item_grade = atoi(values[0]);
        records[count].top = atoi(values[1]);
        records[count].bottom = atoi(values[2]);
        records[count].weight = atoi(values[3]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemSelectStd_GetTotalWeight(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int total_weight = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT SUM(weight) FROM item_select_std");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) {
            total_weight = atoi(values[0]);
        } else {
            total_weight = 0;
        }
    }

    DBQueryResult_Free(&result);
    return total_weight;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void ItemSelectStd_PrintInfo(const ItemSelectStd* record) {
    if (!record) {
        printf("ItemSelectStd: NULL\n");
        return;
    }

    printf("=== Item Select Standard ===\n");
    printf("  item_grade: %d\n", record->item_grade);
    printf("  top: %d\n", record->top);
    printf("  bottom: %d\n", record->bottom);
    printf("  weight: %d\n", record->weight);
    printf("  range: [%d, %d]\n", record->bottom, record->top);
    printf("============================\n");
}
