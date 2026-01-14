#include "auto_market_condition_ctrl_daily_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int AutoMarketConditionCtrlDaily_Add(DBConnectionManager* manager, const AutoMarketConditionCtrlDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO auto_market_condition_ctrl_daily "
        "(occ_time, total_gold, auction_gold, over_gold, optimum_gold_supply, "
        "gold_phase, item_phase, durability_phase) VALUES "
        "('%s', %llu, %llu, %llu, %llu, %d, %d, %u)",
        record->occ_time, record->total_gold, record->auction_gold, record->over_gold,
        record->optimum_gold_supply, record->gold_phase, record->item_phase, record->durability_phase);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlDaily_Get(DBConnectionManager* manager, const char* occ_time, AutoMarketConditionCtrlDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[8];

    if (!manager || !occ_time || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold, auction_gold, over_gold, optimum_gold_supply, "
        "gold_phase, item_phase, durability_phase "
        "FROM auto_market_condition_ctrl_daily WHERE occ_time = '%s'", occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(record->occ_time, values[0], sizeof(record->occ_time) - 1);
        record->total_gold = strtoull(values[1], NULL, 10);
        record->auction_gold = strtoull(values[2], NULL, 10);
        record->over_gold = strtoull(values[3], NULL, 10);
        record->optimum_gold_supply = strtoull(values[4], NULL, 10);
        record->gold_phase = atoi(values[5]);
        record->item_phase = atoi(values[6]);
        record->durability_phase = (unsigned int)atoi(values[7]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int AutoMarketConditionCtrlDaily_Update(DBConnectionManager* manager, const AutoMarketConditionCtrlDaily* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE auto_market_condition_ctrl_daily SET "
        "total_gold = %llu, auction_gold = %llu, over_gold = %llu, optimum_gold_supply = %llu, "
        "gold_phase = %d, item_phase = %d, durability_phase = %u WHERE occ_time = '%s'",
        record->total_gold, record->auction_gold, record->over_gold, record->optimum_gold_supply,
        record->gold_phase, record->item_phase, record->durability_phase, record->occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlDaily_Delete(DBConnectionManager* manager, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !occ_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM auto_market_condition_ctrl_daily WHERE occ_time = '%s'", occ_time);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlDaily_Exists(DBConnectionManager* manager, const char* occ_time) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !occ_time) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM auto_market_condition_ctrl_daily WHERE occ_time = '%s'", occ_time);

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

int AutoMarketConditionCtrlDaily_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM auto_market_condition_ctrl_daily");

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

int AutoMarketConditionCtrlDaily_GetAll(DBConnectionManager* manager, AutoMarketConditionCtrlDaily* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[8];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold, auction_gold, over_gold, optimum_gold_supply, "
        "gold_phase, item_phase, durability_phase FROM auto_market_condition_ctrl_daily "
        "ORDER BY occ_time DESC LIMIT %d", max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold = strtoull(values[1], NULL, 10);
        records[count].auction_gold = strtoull(values[2], NULL, 10);
        records[count].over_gold = strtoull(values[3], NULL, 10);
        records[count].optimum_gold_supply = strtoull(values[4], NULL, 10);
        records[count].gold_phase = atoi(values[5]);
        records[count].item_phase = atoi(values[6]);
        records[count].durability_phase = (unsigned int)atoi(values[7]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AutoMarketConditionCtrlDaily_GetByDateRange(DBConnectionManager* manager,
                                                 const char* start_date,
                                                 const char* end_date,
                                                 AutoMarketConditionCtrlDaily* records,
                                                 int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[8];
    int count = 0;

    if (!manager || !start_date || !end_date || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold, auction_gold, over_gold, optimum_gold_supply, "
        "gold_phase, item_phase, durability_phase FROM auto_market_condition_ctrl_daily "
        "WHERE occ_time BETWEEN '%s' AND '%s' ORDER BY occ_time DESC LIMIT %d",
        start_date, end_date, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold = strtoull(values[1], NULL, 10);
        records[count].auction_gold = strtoull(values[2], NULL, 10);
        records[count].over_gold = strtoull(values[3], NULL, 10);
        records[count].optimum_gold_supply = strtoull(values[4], NULL, 10);
        records[count].gold_phase = atoi(values[5]);
        records[count].item_phase = atoi(values[6]);
        records[count].durability_phase = (unsigned int)atoi(values[7]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int AutoMarketConditionCtrlDaily_GetRecent(DBConnectionManager* manager,
                                           int days,
                                           AutoMarketConditionCtrlDaily* records,
                                           int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[8];
    int count = 0;

    if (!manager || days <= 0 || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT occ_time, total_gold, auction_gold, over_gold, optimum_gold_supply, "
        "gold_phase, item_phase, durability_phase FROM auto_market_condition_ctrl_daily "
        "WHERE occ_time >= DATE_SUB(CURDATE(), INTERVAL %d DAY) "
        "ORDER BY occ_time DESC LIMIT %d", days, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) strncpy(records[count].occ_time, values[0], sizeof(records[count].occ_time) - 1);
        records[count].total_gold = strtoull(values[1], NULL, 10);
        records[count].auction_gold = strtoull(values[2], NULL, 10);
        records[count].over_gold = strtoull(values[3], NULL, 10);
        records[count].optimum_gold_supply = strtoull(values[4], NULL, 10);
        records[count].gold_phase = atoi(values[5]);
        records[count].item_phase = atoi(values[6]);
        records[count].durability_phase = (unsigned int)atoi(values[7]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 统计操作实现
 ******************************************************************************/

long long AutoMarketConditionCtrlDaily_GetAverageTotalGold(DBConnectionManager* manager, int days) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || days <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT AVG(total_gold) FROM auto_market_condition_ctrl_daily "
        "WHERE occ_time >= DATE_SUB(CURDATE(), INTERVAL %d DAY)", days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0]) {
        long long avg = (long long)strtoull(values[0], NULL, 10);
        DBQueryResult_Free(&result);
        return avg;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrlDaily_GetGoldPhaseCount(DBConnectionManager* manager, int phase, int days) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || days <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM auto_market_condition_ctrl_daily "
        "WHERE gold_phase = %d AND occ_time >= DATE_SUB(CURDATE(), INTERVAL %d DAY)",
        phase, days);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        int count = atoi(values[0]);
        DBQueryResult_Free(&result);
        return count;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void AutoMarketConditionCtrlDaily_PrintInfo(const AutoMarketConditionCtrlDaily* record) {
    if (!record) {
        printf("AutoMarketConditionCtrlDaily: NULL\n");
        return;
    }

    printf("\n========== 每日市场统计 ==========\n");
    printf("日期:           %s\n", record->occ_time);
    printf("金币总量:       %llu\n", record->total_gold);
    printf("拍卖行金币:     %llu\n", record->auction_gold);
    printf("超额金币:       %llu\n", record->over_gold);
    printf("最优供应量:     %llu\n", record->optimum_gold_supply);

    if (record->optimum_gold_supply > 0) {
        double over_pct = (double)record->over_gold / record->optimum_gold_supply * 100.0;
        printf("超额百分比:     %.2f%%\n", over_pct);
    }

    printf("金币阶段:       %d\n", record->gold_phase);
    printf("物品阶段:       %d\n", record->item_phase);
    printf("耐久度阶段:     %u\n", record->durability_phase);
    printf("==================================\n\n");
}
