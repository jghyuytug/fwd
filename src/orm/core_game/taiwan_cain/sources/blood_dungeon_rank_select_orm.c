#include "blood_dungeon_rank_select_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int BloodDungeonRankSelect_Add(DBConnectionManager* manager, const BloodDungeonRankSelect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO blood_dungeon_rank_select "
        "(min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count) VALUES "
        "(%lld, %lld, %u, %u, %u, %u)",
        record->min_amount, record->max_amount, record->rank,
        record->reward_item_id, record->reward_gold, record->winner_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BloodDungeonRankSelect_Get(DBConnectionManager* manager, long long min_amount, long long max_amount,
                                unsigned char rank, BloodDungeonRankSelect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count "
        "FROM blood_dungeon_rank_select "
        "WHERE min_amount = %lld AND max_amount = %lld AND rank = %u",
        min_amount, max_amount, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->min_amount = strtoll(values[0], NULL, 10);
        record->max_amount = strtoll(values[1], NULL, 10);
        record->rank = (unsigned char)atoi(values[2]);
        record->reward_item_id = (unsigned int)atoi(values[3]);
        record->reward_gold = (unsigned int)atoi(values[4]);
        record->winner_count = (unsigned int)atoi(values[5]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int BloodDungeonRankSelect_Update(DBConnectionManager* manager, const BloodDungeonRankSelect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE blood_dungeon_rank_select SET "
        "reward_item_id = %u, reward_gold = %u, winner_count = %u "
        "WHERE min_amount = %lld AND max_amount = %lld AND rank = %u",
        record->reward_item_id, record->reward_gold, record->winner_count,
        record->min_amount, record->max_amount, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BloodDungeonRankSelect_Delete(DBConnectionManager* manager, long long min_amount, long long max_amount, unsigned char rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM blood_dungeon_rank_select "
        "WHERE min_amount = %lld AND max_amount = %lld AND rank = %u",
        min_amount, max_amount, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int BloodDungeonRankSelect_Exists(DBConnectionManager* manager, long long min_amount, long long max_amount, unsigned char rank) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM blood_dungeon_rank_select "
        "WHERE min_amount = %lld AND max_amount = %lld AND rank = %u",
        min_amount, max_amount, rank);

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

int BloodDungeonRankSelect_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM blood_dungeon_rank_select");

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

int BloodDungeonRankSelect_GetAll(DBConnectionManager* manager, BloodDungeonRankSelect* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count "
        "FROM blood_dungeon_rank_select "
        "ORDER BY min_amount, max_amount, rank LIMIT %d", max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].min_amount = strtoll(values[0], NULL, 10);
        records[count].max_amount = strtoll(values[1], NULL, 10);
        records[count].rank = (unsigned char)atoi(values[2]);
        records[count].reward_item_id = (unsigned int)atoi(values[3]);
        records[count].reward_gold = (unsigned int)atoi(values[4]);
        records[count].winner_count = (unsigned int)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int BloodDungeonRankSelect_GetByPointRange(DBConnectionManager* manager, long long min_amount, long long max_amount,
                                            BloodDungeonRankSelect* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count "
        "FROM blood_dungeon_rank_select "
        "WHERE min_amount = %lld AND max_amount = %lld "
        "ORDER BY rank LIMIT %d",
        min_amount, max_amount, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].min_amount = strtoll(values[0], NULL, 10);
        records[count].max_amount = strtoll(values[1], NULL, 10);
        records[count].rank = (unsigned char)atoi(values[2]);
        records[count].reward_item_id = (unsigned int)atoi(values[3]);
        records[count].reward_gold = (unsigned int)atoi(values[4]);
        records[count].winner_count = (unsigned int)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int BloodDungeonRankSelect_GetByRank(DBConnectionManager* manager, unsigned char rank,
                                      BloodDungeonRankSelect* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count "
        "FROM blood_dungeon_rank_select "
        "WHERE rank = %u "
        "ORDER BY min_amount, max_amount LIMIT %d",
        rank, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].min_amount = strtoll(values[0], NULL, 10);
        records[count].max_amount = strtoll(values[1], NULL, 10);
        records[count].rank = (unsigned char)atoi(values[2]);
        records[count].reward_item_id = (unsigned int)atoi(values[3]);
        records[count].reward_gold = (unsigned int)atoi(values[4]);
        records[count].winner_count = (unsigned int)atoi(values[5]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int BloodDungeonRankSelect_FindRewardByPoint(DBConnectionManager* manager, long long point, unsigned char rank,
                                              BloodDungeonRankSelect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[6];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT min_amount, max_amount, rank, reward_item_id, reward_gold, winner_count "
        "FROM blood_dungeon_rank_select "
        "WHERE %lld >= min_amount AND %lld <= max_amount AND rank = %u "
        "LIMIT 1",
        point, point, rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->min_amount = strtoll(values[0], NULL, 10);
        record->max_amount = strtoll(values[1], NULL, 10);
        record->rank = (unsigned char)atoi(values[2]);
        record->reward_item_id = (unsigned int)atoi(values[3]);
        record->reward_gold = (unsigned int)atoi(values[4]);
        record->winner_count = (unsigned int)atoi(values[5]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/*******************************************************************************
 * 统计操作实现
 ******************************************************************************/

int BloodDungeonRankSelect_CountPointRanges(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(DISTINCT CONCAT(min_amount, '-', max_amount)) "
        "FROM blood_dungeon_rank_select");

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

void BloodDungeonRankSelect_PrintInfo(const BloodDungeonRankSelect* record) {
    if (!record) {
        printf("BloodDungeonRankSelect: NULL\n");
        return;
    }

    printf("\n========== 血色副本排名奖励配置 ==========\n");
    printf("积分区间:       %lld ~ %lld\n", record->min_amount, record->max_amount);
    printf("排名:           %u\n", record->rank);
    printf("奖励物品ID:     %u\n", record->reward_item_id);
    printf("奖励金币:       %u\n", record->reward_gold);
    printf("获奖人数:       %u\n", record->winner_count);
    printf("========================================\n\n");
}
