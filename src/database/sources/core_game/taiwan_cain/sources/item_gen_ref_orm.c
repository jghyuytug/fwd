#include "item_gen_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int ItemGenRef_Add(DBConnectionManager* manager, const ItemGenRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO item_gen_ref (item_grade, rate_type, money_rate, item_rate, free_rate) VALUES (%d, %d, %d, %d, %d)",
        record->item_grade, record->rate_type, record->money_rate, record->item_rate, record->free_rate);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemGenRef_Get(DBConnectionManager* manager, signed char item_grade, signed char rate_type, ItemGenRef* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(ItemGenRef));

    snprintf(query, sizeof(query),
        "SELECT item_grade, rate_type, money_rate, item_rate, free_rate FROM item_gen_ref WHERE item_grade = %d AND rate_type = %d",
        item_grade, rate_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->item_grade = (signed char)atoi(values[0]);
        record->rate_type = (signed char)atoi(values[1]);
        record->money_rate = (short)atoi(values[2]);
        record->item_rate = (short)atoi(values[3]);
        record->free_rate = (short)atoi(values[4]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int ItemGenRef_Update(DBConnectionManager* manager, const ItemGenRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE item_gen_ref SET money_rate = %d, item_rate = %d, free_rate = %d WHERE item_grade = %d AND rate_type = %d",
        record->money_rate, record->item_rate, record->free_rate, record->item_grade, record->rate_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemGenRef_Delete(DBConnectionManager* manager, signed char item_grade, signed char rate_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM item_gen_ref WHERE item_grade = %d AND rate_type = %d",
        item_grade, rate_type);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int ItemGenRef_Exists(DBConnectionManager* manager, signed char item_grade, signed char rate_type) {
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
        "SELECT COUNT(*) FROM item_gen_ref WHERE item_grade = %d AND rate_type = %d",
        item_grade, rate_type);

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

int ItemGenRef_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM item_gen_ref");

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

int ItemGenRef_GetAll(DBConnectionManager* manager, ItemGenRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT item_grade, rate_type, money_rate, item_rate, free_rate FROM item_gen_ref ORDER BY item_grade, rate_type LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].item_grade = (signed char)atoi(values[0]);
        records[count].rate_type = (signed char)atoi(values[1]);
        records[count].money_rate = (short)atoi(values[2]);
        records[count].item_rate = (short)atoi(values[3]);
        records[count].free_rate = (short)atoi(values[4]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemGenRef_GetByGrade(DBConnectionManager* manager, signed char item_grade, ItemGenRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT item_grade, rate_type, money_rate, item_rate, free_rate FROM item_gen_ref WHERE item_grade = %d ORDER BY rate_type LIMIT %d",
        item_grade, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].item_grade = (signed char)atoi(values[0]);
        records[count].rate_type = (signed char)atoi(values[1]);
        records[count].money_rate = (short)atoi(values[2]);
        records[count].item_rate = (short)atoi(values[3]);
        records[count].free_rate = (short)atoi(values[4]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int ItemGenRef_CountByGrade(DBConnectionManager* manager, signed char item_grade) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM item_gen_ref WHERE item_grade = %d",
        item_grade);

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

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int ItemGenRef_BatchDelete(DBConnectionManager* manager, const ItemGenRef* records, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        memset(&result, 0, sizeof(DBQueryResult));

        snprintf(query, sizeof(query),
            "DELETE FROM item_gen_ref WHERE item_grade = %d AND rate_type = %d",
            records[i].item_grade, records[i].rate_type);

        if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
            DBQueryResult_Free(&result);
            return -1;
        }

        DBQueryResult_Free(&result);
    }

    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void ItemGenRef_PrintInfo(const ItemGenRef* record) {
    if (!record) {
        printf("ItemGenRef: NULL\n");
        return;
    }

    printf("=== Item Gen Ref ===\n");
    printf("  item_grade: %d\n", record->item_grade);
    printf("  rate_type: %d\n", record->rate_type);
    printf("  money_rate: %d\n", record->money_rate);
    printf("  item_rate: %d\n", record->item_rate);
    printf("  free_rate: %d\n", record->free_rate);
    printf("====================\n");
}
