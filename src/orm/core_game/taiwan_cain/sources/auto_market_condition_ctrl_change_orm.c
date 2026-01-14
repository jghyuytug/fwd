#include "auto_market_condition_ctrl_change_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int AutoMarketConditionCtrlChange_Add(DBConnectionManager* manager, const AutoMarketConditionCtrlChange* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO auto_market_condition_ctrl_change "
        "(occ_time, total_gold_old, over_gold_old, total_gold_new, over_gold_new, MNG_user_id, memo) "
        "VALUES ('%s', %llu, %llu, %llu, %llu, '%s', '%s')",
        record->occ_time, record->total_gold_old, record->over_gold_old,
        record->total_gold_new, record->over_gold_new, record->MNG_user_id, record->memo);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlChange_Get(DBConnectionManager* manager, const char* occ_time, AutoMarketConditionCtrlChange* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[7];

    if (!manager || !occ_time || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold_old, over_gold_old, total_gold_new, over_gold_new, MNG_user_id, memo "
        "FROM auto_market_condition_ctrl_change WHERE occ_time = '%s'", occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(record->occ_time, values[0], sizeof(record->occ_time) - 1);
        record->total_gold_old = strtoull(values[1], NULL, 10);
        record->over_gold_old = strtoull(values[2], NULL, 10);
        record->total_gold_new = strtoull(values[3], NULL, 10);
        record->over_gold_new = strtoull(values[4], NULL, 10);
        if (values[5]) strncpy(record->MNG_user_id, values[5], sizeof(record->MNG_user_id) - 1);
        if (values[6]) strncpy(record->memo, values[6], sizeof(record->memo) - 1);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int AutoMarketConditionCtrlChange_Update(DBConnectionManager* manager, const AutoMarketConditionCtrlChange* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE auto_market_condition_ctrl_change SET "
        "total_gold_old = %llu, over_gold_old = %llu, total_gold_new = %llu, "
        "over_gold_new = %llu, MNG_user_id = '%s', memo = '%s' WHERE occ_time = '%s'",
        record->total_gold_old, record->over_gold_old, record->total_gold_new,
        record->over_gold_new, record->MNG_user_id, record->memo, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlChange_Delete(DBConnectionManager* manager, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM auto_market_condition_ctrl_change WHERE occ_time = '%s'", occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlChange_Exists(DBConnectionManager* manager, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !occ_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM auto_market_condition_ctrl_change WHERE occ_time = '%s'", occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return (count > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int AutoMarketConditionCtrlChange_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM auto_market_condition_ctrl_change");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int AutoMarketConditionCtrlChange_GetAll(DBConnectionManager* manager, AutoMarketConditionCtrlChange* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[7];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold_old, over_gold_old, total_gold_new, over_gold_new, MNG_user_id, memo "
        "FROM auto_market_condition_ctrl_change ORDER BY occ_time DESC LIMIT %d", max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold_old = strtoull(values[1], NULL, 10);
        records[count].over_gold_old = strtoull(values[2], NULL, 10);
        records[count].total_gold_new = strtoull(values[3], NULL, 10);
        records[count].over_gold_new = strtoull(values[4], NULL, 10);
        if (values[5]) strncpy(records[count].MNG_user_id, values[5], sizeof(records[count].MNG_user_id) - 1);
        if (values[6]) strncpy(records[count].memo, values[6], sizeof(records[count].memo) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AutoMarketConditionCtrlChange_GetByDateRange(DBConnectionManager* manager,
                                                  const char* start_date,
                                                  const char* end_date,
                                                  AutoMarketConditionCtrlChange* records,
                                                  int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[7];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold_old, over_gold_old, total_gold_new, over_gold_new, MNG_user_id, memo "
        "FROM auto_market_condition_ctrl_change WHERE occ_time BETWEEN '%s' AND '%s' "
        "ORDER BY occ_time DESC LIMIT %d", start_date, end_date, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold_old = strtoull(values[1], NULL, 10);
        records[count].over_gold_old = strtoull(values[2], NULL, 10);
        records[count].total_gold_new = strtoull(values[3], NULL, 10);
        records[count].over_gold_new = strtoull(values[4], NULL, 10);
        if (values[5]) strncpy(records[count].MNG_user_id, values[5], sizeof(records[count].MNG_user_id) - 1);
        if (values[6]) strncpy(records[count].memo, values[6], sizeof(records[count].memo) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AutoMarketConditionCtrlChange_GetByUser(DBConnectionManager* manager,
                                             const char* user_id,
                                             AutoMarketConditionCtrlChange* records,
                                             int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[7];
    int count = 0;

    if (!manager || !user_id || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold_old, over_gold_old, total_gold_new, over_gold_new, MNG_user_id, memo "
        "FROM auto_market_condition_ctrl_change WHERE MNG_user_id = '%s' "
        "ORDER BY occ_time DESC LIMIT %d", user_id, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold_old = strtoull(values[1], NULL, 10);
        records[count].over_gold_old = strtoull(values[2], NULL, 10);
        records[count].total_gold_new = strtoull(values[3], NULL, 10);
        records[count].over_gold_new = strtoull(values[4], NULL, 10);
        if (values[5]) strncpy(records[count].MNG_user_id, values[5], sizeof(records[count].MNG_user_id) - 1);
        if (values[6]) strncpy(records[count].memo, values[6], sizeof(records[count].memo) - 1);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 统计操作实现
 ******************************************************************************/

int AutoMarketConditionCtrlChange_CountRecentChanges(DBConnectionManager* manager, int days) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || days <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM auto_market_condition_ctrl_change "
        "WHERE occ_time >= DATE_SUB(CURDATE(), INTERVAL %d DAY)", days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void AutoMarketConditionCtrlChange_PrintInfo(const AutoMarketConditionCtrlChange* record) {
    if (!record) {
        printf("AutoMarketConditionCtrlChange: NULL\n");
        return;
    }

    printf("\n========== 市场条件控制变更记录 ==========\n");
    printf("发生日期:       %s\n", record->occ_time);
    printf("变更前金币总量: %llu\n", record->total_gold_old);
    printf("变更前超额:     %llu\n", record->over_gold_old);
    printf("变更后金币总量: %llu\n", record->total_gold_new);
    printf("变更后超额:     %llu\n", record->over_gold_new);

    /* 计算变化量和百分比 */
    long long total_diff = (long long)record->total_gold_new - (long long)record->total_gold_old;
    long long over_diff = (long long)record->over_gold_new - (long long)record->over_gold_old;

    printf("金币总量变化:   %+lld", total_diff);
    if (record->total_gold_old > 0) {
        double total_pct = (double)total_diff / record->total_gold_old * 100.0;
        printf(" (%+.2f%%)", total_pct);
    }
    printf("\n");

    printf("超额变化:       %+lld", over_diff);
    if (record->over_gold_old > 0) {
        double over_pct = (double)over_diff / record->over_gold_old * 100.0;
        printf(" (%+.2f%%)", over_pct);
    }
    printf("\n");

    printf("操作管理员:     %s\n", record->MNG_user_id);
    printf("备注:           %s\n", record->memo);
    printf("==========================================\n\n");
}
