#include "auto_market_condition_ctrl_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 1024

/*******************************************************************************
 * 配置操作实现
 ******************************************************************************/

int AutoMarketConditionCtrl_Get(DBConnectionManager* manager, AutoMarketConditionCtrl* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[2];

    if (!manager || !config) {
        return -1;
    }

    /* 查询唯一记录（无WHERE条件） */
    snprintf(query, sizeof(query),
        "SELECT optimum_gold_supply, over_gold FROM auto_market_condition_ctrl LIMIT 1");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    /* 尝试获取第一条（也是唯一一条）记录 */
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        config->optimum_gold_supply = strtoull(values[0], NULL, 10);
        config->over_gold = strtoull(values[1], NULL, 10);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;  /* 表为空 */
}

int AutoMarketConditionCtrl_Update(DBConnectionManager* manager, const AutoMarketConditionCtrl* config) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !config) {
        return -1;
    }

    /* 先检查是否存在记录 */
    int exists = AutoMarketConditionCtrl_Exists(manager);
    if (exists < 0) {
        return -1;
    }

    if (exists == 0) {
        /* 表为空，插入新记录 */
        snprintf(query, sizeof(query),
            "INSERT INTO auto_market_condition_ctrl (optimum_gold_supply, over_gold) VALUES (%llu, %llu)",
            config->optimum_gold_supply, config->over_gold);
    } else {
        /* 表有记录，更新（无WHERE条件，更新唯一行） */
        snprintf(query, sizeof(query),
            "UPDATE auto_market_condition_ctrl SET optimum_gold_supply = %llu, over_gold = %llu",
            config->optimum_gold_supply, config->over_gold);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrl_Initialize(DBConnectionManager* manager,
                                       unsigned long long optimum_gold_supply,
                                       unsigned long long over_gold) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    /* 检查是否已存在配置 */
    if (AutoMarketConditionCtrl_Exists(manager) > 0) {
        return 0;  /* 已存在，无需初始化 */
    }

    /* 插入默认配置 */
    snprintf(query, sizeof(query),
        "INSERT INTO auto_market_condition_ctrl (optimum_gold_supply, over_gold) VALUES (%llu, %llu)",
        optimum_gold_supply, over_gold);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 金币操作实现
 ******************************************************************************/

int AutoMarketConditionCtrl_IncreaseSupply(DBConnectionManager* manager, unsigned long long amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || amount == 0) {
        return -1;
    }

    /* 原子递增操作（无WHERE条件，更新唯一行） */
    snprintf(query, sizeof(query),
        "UPDATE auto_market_condition_ctrl SET optimum_gold_supply = optimum_gold_supply + %llu",
        amount);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AutoMarketConditionCtrl_DecreaseSupply(DBConnectionManager* manager, unsigned long long amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || amount == 0) {
        return -1;
    }

    /* 原子递减操作（无WHERE条件，更新唯一行），防止下溢 */
    snprintf(query, sizeof(query),
        "UPDATE auto_market_condition_ctrl SET optimum_gold_supply = "
        "CASE WHEN optimum_gold_supply >= %llu THEN optimum_gold_supply - %llu ELSE 0 END",
        amount, amount);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

int AutoMarketConditionCtrl_Exists(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    /* 检查表中是否有记录 */
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM auto_market_condition_ctrl");

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

void AutoMarketConditionCtrl_PrintInfo(const AutoMarketConditionCtrl* config) {
    if (!config) {
        printf("AutoMarketConditionCtrl: NULL\n");
        return;
    }

    printf("\n========== 自动市场条件控制配置 ==========\n");
    printf("最优金币供应量: %llu\n", config->optimum_gold_supply);
    printf("超额金币量:     %llu\n", config->over_gold);

    /* 计算超额百分比 */
    if (config->optimum_gold_supply > 0) {
        double over_percentage = (double)config->over_gold / config->optimum_gold_supply * 100.0;
        printf("超额百分比:     %.2f%%\n", over_percentage);

        if (over_percentage > 20.0) {
            printf("状态: ⚠️  通货膨胀风险（超额>20%%）\n");
        } else if (over_percentage < -20.0) {
            printf("状态: ⚠️  通货紧缩风险（超额<-20%%）\n");
        } else {
            printf("状态: ✅ 金币供应健康\n");
        }
    }
    printf("==========================================\n\n");
}
