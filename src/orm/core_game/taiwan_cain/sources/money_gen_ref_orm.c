#include "money_gen_ref_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int MoneyGenRef_Add(DBConnectionManager* manager, const MoneyGenRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO money_gen_ref (grade, bottom_grade, money, random_value) VALUES (%d, %d, %d, %d)",
        record->grade, record->bottom_grade, record->money, record->random_value);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MoneyGenRef_Get(DBConnectionManager* manager, int grade, int bottom_grade, MoneyGenRef* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(MoneyGenRef));

    snprintf(query, sizeof(query),
        "SELECT grade, bottom_grade, money, random_value FROM money_gen_ref WHERE grade = %d AND bottom_grade = %d",
        grade, bottom_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->grade = atoi(values[0]);
        record->bottom_grade = atoi(values[1]);
        record->money = atoi(values[2]);
        record->random_value = atoi(values[3]);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int MoneyGenRef_Update(DBConnectionManager* manager, const MoneyGenRef* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE money_gen_ref SET money = %d, random_value = %d WHERE grade = %d AND bottom_grade = %d",
        record->money, record->random_value, record->grade, record->bottom_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MoneyGenRef_Delete(DBConnectionManager* manager, int grade, int bottom_grade) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM money_gen_ref WHERE grade = %d AND bottom_grade = %d",
        grade, bottom_grade);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MoneyGenRef_Exists(DBConnectionManager* manager, int grade, int bottom_grade) {
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
        "SELECT COUNT(*) FROM money_gen_ref WHERE grade = %d AND bottom_grade = %d",
        grade, bottom_grade);

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

int MoneyGenRef_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM money_gen_ref");

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

int MoneyGenRef_GetAll(DBConnectionManager* manager, MoneyGenRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT grade, bottom_grade, money, random_value FROM money_gen_ref ORDER BY grade, bottom_grade LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].grade = atoi(values[0]);
        records[count].bottom_grade = atoi(values[1]);
        records[count].money = atoi(values[2]);
        records[count].random_value = atoi(values[3]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MoneyGenRef_GetByGrade(DBConnectionManager* manager, int grade, MoneyGenRef* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT grade, bottom_grade, money, random_value FROM money_gen_ref WHERE grade = %d ORDER BY bottom_grade LIMIT %d",
        grade, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].grade = atoi(values[0]);
        records[count].bottom_grade = atoi(values[1]);
        records[count].money = atoi(values[2]);
        records[count].random_value = atoi(values[3]);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int MoneyGenRef_CountByGrade(DBConnectionManager* manager, int grade) {
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
        "SELECT COUNT(*) FROM money_gen_ref WHERE grade = %d",
        grade);

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

int MoneyGenRef_BatchDelete(DBConnectionManager* manager, const MoneyGenRef* records, int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    for (i = 0; i < count; i++) {
        memset(&result, 0, sizeof(DBQueryResult));

        snprintf(query, sizeof(query),
            "DELETE FROM money_gen_ref WHERE grade = %d AND bottom_grade = %d",
            records[i].grade, records[i].bottom_grade);

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

void MoneyGenRef_PrintInfo(const MoneyGenRef* record) {
    if (!record) {
        printf("MoneyGenRef: NULL\n");
        return;
    }

    printf("=== Money Gen Ref ===\n");
    printf("  grade: %d\n", record->grade);
    printf("  bottom_grade: %d\n", record->bottom_grade);
    printf("  money: %d\n", record->money);
    printf("  random_value: %d\n", record->random_value);
    printf("=====================\n");
}
