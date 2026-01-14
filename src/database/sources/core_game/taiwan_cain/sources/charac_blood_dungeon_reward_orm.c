#include "charac_blood_dungeon_reward_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 2048

/* ==================== CRUD 操作实现 ==================== */

int CharacBloodDungeonReward_Add(DBConnectionManager* manager, const CharacBloodDungeonReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_blood_dungeon_reward "
        "(charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold) "
        "VALUES (%u, '%s', %u, %u, %u, '%s', %u, %u, %u, %u, %u)",
        record->charac_no, record->week_occ_date, record->week_point,
        record->week_enter_count, record->week_use_gold, record->last_play_date,
        record->enter_count, record->rank, record->reward,
        record->reward_item_id, record->reward_gold);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodDungeonReward_Get(DBConnectionManager* manager, unsigned int charac_no,
                                  const char* week_occ_date, CharacBloodDungeonReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];

    if (!manager || !week_occ_date || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE charac_no = %u AND week_occ_date = '%s'",
        charac_no, week_occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)atoi(values[0]);
        strncpy(record->week_occ_date, values[1], sizeof(record->week_occ_date) - 1);
        record->week_occ_date[sizeof(record->week_occ_date) - 1] = '\0';
        record->week_point = (unsigned int)atoi(values[2]);
        record->week_enter_count = (unsigned int)atoi(values[3]);
        record->week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(record->last_play_date, values[5], sizeof(record->last_play_date) - 1);
        record->last_play_date[sizeof(record->last_play_date) - 1] = '\0';
        record->enter_count = (unsigned int)atoi(values[6]);
        record->rank = (unsigned char)atoi(values[7]);
        record->reward = (unsigned char)atoi(values[8]);
        record->reward_item_id = (unsigned int)atoi(values[9]);
        record->reward_gold = (unsigned int)atoi(values[10]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

int CharacBloodDungeonReward_Update(DBConnectionManager* manager, const CharacBloodDungeonReward* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE charac_blood_dungeon_reward SET "
        "week_point = %u, week_enter_count = %u, week_use_gold = %u, "
        "last_play_date = '%s', enter_count = %u, rank = %u, "
        "reward = %u, reward_item_id = %u, reward_gold = %u "
        "WHERE charac_no = %u AND week_occ_date = '%s'",
        record->week_point, record->week_enter_count, record->week_use_gold,
        record->last_play_date, record->enter_count, record->rank,
        record->reward, record->reward_item_id, record->reward_gold,
        record->charac_no, record->week_occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodDungeonReward_Delete(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* week_occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !week_occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_blood_dungeon_reward WHERE charac_no = %u AND week_occ_date = '%s'",
        charac_no, week_occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacBloodDungeonReward_Exists(DBConnectionManager* manager, unsigned int charac_no,
                                     const char* week_occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];

    if (!manager || !week_occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_dungeon_reward WHERE charac_no = %u AND week_occ_date = '%s'",
        charac_no, week_occ_date);

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

/* ==================== 查询操作实现 ==================== */

int CharacBloodDungeonReward_GetByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                          CharacBloodDungeonReward* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE charac_no = %u "
        "ORDER BY week_occ_date DESC LIMIT %d",
        charac_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].week_occ_date, values[1], sizeof(records[count].week_occ_date) - 1);
        records[count].week_occ_date[sizeof(records[count].week_occ_date) - 1] = '\0';
        records[count].week_point = (unsigned int)atoi(values[2]);
        records[count].week_enter_count = (unsigned int)atoi(values[3]);
        records[count].week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(records[count].last_play_date, values[5], sizeof(records[count].last_play_date) - 1);
        records[count].last_play_date[sizeof(records[count].last_play_date) - 1] = '\0';
        records[count].enter_count = (unsigned int)atoi(values[6]);
        records[count].rank = (unsigned char)atoi(values[7]);
        records[count].reward = (unsigned char)atoi(values[8]);
        records[count].reward_item_id = (unsigned int)atoi(values[9]);
        records[count].reward_gold = (unsigned int)atoi(values[10]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_GetByWeekOccDate(DBConnectionManager* manager, const char* week_occ_date,
                                               CharacBloodDungeonReward* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];
    int count = 0;

    if (!manager || !week_occ_date || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE week_occ_date = '%s' "
        "ORDER BY week_point DESC LIMIT %d",
        week_occ_date, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].week_occ_date, values[1], sizeof(records[count].week_occ_date) - 1);
        records[count].week_occ_date[sizeof(records[count].week_occ_date) - 1] = '\0';
        records[count].week_point = (unsigned int)atoi(values[2]);
        records[count].week_enter_count = (unsigned int)atoi(values[3]);
        records[count].week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(records[count].last_play_date, values[5], sizeof(records[count].last_play_date) - 1);
        records[count].last_play_date[sizeof(records[count].last_play_date) - 1] = '\0';
        records[count].enter_count = (unsigned int)atoi(values[6]);
        records[count].rank = (unsigned char)atoi(values[7]);
        records[count].reward = (unsigned char)atoi(values[8]);
        records[count].reward_item_id = (unsigned int)atoi(values[9]);
        records[count].reward_gold = (unsigned int)atoi(values[10]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_GetByLastPlayDate(DBConnectionManager* manager, const char* last_play_date,
                                                 CharacBloodDungeonReward* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];
    int count = 0;

    if (!manager || !last_play_date || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE last_play_date = '%s' "
        "ORDER BY week_point DESC LIMIT %d",
        last_play_date, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].week_occ_date, values[1], sizeof(records[count].week_occ_date) - 1);
        records[count].week_occ_date[sizeof(records[count].week_occ_date) - 1] = '\0';
        records[count].week_point = (unsigned int)atoi(values[2]);
        records[count].week_enter_count = (unsigned int)atoi(values[3]);
        records[count].week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(records[count].last_play_date, values[5], sizeof(records[count].last_play_date) - 1);
        records[count].last_play_date[sizeof(records[count].last_play_date) - 1] = '\0';
        records[count].enter_count = (unsigned int)atoi(values[6]);
        records[count].rank = (unsigned char)atoi(values[7]);
        records[count].reward = (unsigned char)atoi(values[8]);
        records[count].reward_item_id = (unsigned int)atoi(values[9]);
        records[count].reward_gold = (unsigned int)atoi(values[10]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_GetUnrewardedByCharac(DBConnectionManager* manager, unsigned int charac_no,
                                                     CharacBloodDungeonReward* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE charac_no = %u AND reward = 0 "
        "ORDER BY week_occ_date DESC LIMIT %d",
        charac_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].week_occ_date, values[1], sizeof(records[count].week_occ_date) - 1);
        records[count].week_occ_date[sizeof(records[count].week_occ_date) - 1] = '\0';
        records[count].week_point = (unsigned int)atoi(values[2]);
        records[count].week_enter_count = (unsigned int)atoi(values[3]);
        records[count].week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(records[count].last_play_date, values[5], sizeof(records[count].last_play_date) - 1);
        records[count].last_play_date[sizeof(records[count].last_play_date) - 1] = '\0';
        records[count].enter_count = (unsigned int)atoi(values[6]);
        records[count].rank = (unsigned char)atoi(values[7]);
        records[count].reward = (unsigned char)atoi(values[8]);
        records[count].reward_item_id = (unsigned int)atoi(values[9]);
        records[count].reward_gold = (unsigned int)atoi(values[10]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_GetTopByWeekPoint(DBConnectionManager* manager, const char* week_occ_date,
                                                 int limit, CharacBloodDungeonReward* records, int max_records) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[11];
    int count = 0;
    int actual_limit;

    if (!manager || !week_occ_date || !records || max_records <= 0) {
        return -1;
    }

    actual_limit = (limit > max_records) ? max_records : limit;

    snprintf(query, sizeof(query),
        "SELECT charac_no, week_occ_date, week_point, week_enter_count, week_use_gold, "
        "last_play_date, enter_count, rank, reward, reward_item_id, reward_gold "
        "FROM charac_blood_dungeon_reward "
        "WHERE week_occ_date = '%s' "
        "ORDER BY week_point DESC LIMIT %d",
        week_occ_date, actual_limit);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)atoi(values[0]);
        strncpy(records[count].week_occ_date, values[1], sizeof(records[count].week_occ_date) - 1);
        records[count].week_occ_date[sizeof(records[count].week_occ_date) - 1] = '\0';
        records[count].week_point = (unsigned int)atoi(values[2]);
        records[count].week_enter_count = (unsigned int)atoi(values[3]);
        records[count].week_use_gold = (unsigned int)atoi(values[4]);
        strncpy(records[count].last_play_date, values[5], sizeof(records[count].last_play_date) - 1);
        records[count].last_play_date[sizeof(records[count].last_play_date) - 1] = '\0';
        records[count].enter_count = (unsigned int)atoi(values[6]);
        records[count].rank = (unsigned char)atoi(values[7]);
        records[count].reward = (unsigned char)atoi(values[8]);
        records[count].reward_item_id = (unsigned int)atoi(values[9]);
        records[count].reward_gold = (unsigned int)atoi(values[10]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

long long CharacBloodDungeonReward_GetTotalWeekPoint(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    long long total = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT SUM(week_point) FROM charac_blood_dungeon_reward WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0] != NULL) {
        total = strtoll(values[0], NULL, 10);
    }

    DBQueryResult_Free(&result);
    return total;
}

int CharacBloodDungeonReward_GetRewardedCount(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_dungeon_reward WHERE charac_no = %u AND reward = 1",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 统计操作实现 ==================== */

int CharacBloodDungeonReward_CountByCharac(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_dungeon_reward WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_CountByWeekOccDate(DBConnectionManager* manager, const char* week_occ_date) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int count = 0;

    if (!manager || !week_occ_date) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_dungeon_reward WHERE week_occ_date = '%s'",
        week_occ_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacBloodDungeonReward_CountByRewardStatus(DBConnectionManager* manager, unsigned char reward_status) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[1];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_blood_dungeon_reward WHERE reward = %u",
        reward_status);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数实现 ==================== */

void CharacBloodDungeonReward_PrintInfo(const CharacBloodDungeonReward* record) {
    if (!record) {
        printf("CharacBloodDungeonReward: NULL\n");
        return;
    }

    printf("CharacBloodDungeonReward {\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  week_occ_date: %s\n", record->week_occ_date);
    printf("  week_point: %u\n", record->week_point);
    printf("  week_enter_count: %u\n", record->week_enter_count);
    printf("  week_use_gold: %u\n", record->week_use_gold);
    printf("  last_play_date: %s\n", record->last_play_date);
    printf("  enter_count: %u\n", record->enter_count);
    printf("  rank: %u\n", record->rank);
    printf("  reward: %u (%s)\n", record->reward, record->reward ? "已领取" : "未领取");
    printf("  reward_item_id: %u\n", record->reward_item_id);
    printf("  reward_gold: %u\n", record->reward_gold);
    printf("}\n");
}
