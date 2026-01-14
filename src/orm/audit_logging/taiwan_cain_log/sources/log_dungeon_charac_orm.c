#include "log_dungeon_charac_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonCharac_Add(DBConnectionManager* manager, const LogDungeonCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_charac (channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, exp_avg, dungeon_standard_level, fatigue_consume, exp_add, party_user_count, rank) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d)",
        record->channel_no, record->dungeon_index, record->dungeon_diff, record->charac_job, record->charac_grow, record->clear_time, record->die_count, record->hp_consume, record->mp_consume, record->hit_count, record->hit_per_avg_damage, record->hp_recovery, record->mp_recovery, record->success, record->last_time, record->update_count, record->level, record->exp_avg, record->dungeon_standard_level, record->fatigue_consume, record->exp_add, record->party_user_count, record->rank);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonCharac_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, int dungeon_standard_level, int charac_job, int charac_grow, int success, signed char party_user_count, LogDungeonCharac* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, exp_avg, dungeon_standard_level, fatigue_consume, exp_add, party_user_count, rank FROM log_dungeon_charac WHERE last_time = '%s' AND channel_no = %d AND dungeon_index = %d AND dungeon_diff = %d AND dungeon_standard_level = %d AND charac_job = %d AND charac_grow = %d AND success = %d AND party_user_count = %d",
        last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, charac_job, charac_grow, success, party_user_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonCharac));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->dungeon_diff = row[2] ? atoi(row[2]) : 0;
    record->charac_job = row[3] ? atoi(row[3]) : 0;
    record->charac_grow = row[4] ? atoi(row[4]) : 0;
    record->clear_time = row[5] ? atoi(row[5]) : 0;
    record->die_count = row[6] ? atoi(row[6]) : 0;
    record->hp_consume = row[7] ? atoi(row[7]) : 0;
    record->mp_consume = row[8] ? atoi(row[8]) : 0;
    record->hit_count = row[9] ? atoi(row[9]) : 0;
    record->hit_per_avg_damage = row[10] ? atoi(row[10]) : 0;
    record->hp_recovery = row[11] ? atoi(row[11]) : 0;
    record->mp_recovery = row[12] ? atoi(row[12]) : 0;
    record->success = row[13] ? atoi(row[13]) : 0;
    if (row[14]) strncpy(record->last_time, row[14], sizeof(record->last_time) - 1);
    record->update_count = row[15] ? atoi(row[15]) : 0;
    record->level = row[16] ? atoi(row[16]) : 0;
    record->exp_avg = row[17] ? atoi(row[17]) : 0;
    record->dungeon_standard_level = row[18] ? atoi(row[18]) : 0;
    record->fatigue_consume = row[19] ? atoi(row[19]) : 0;
    record->exp_add = row[20] ? atoi(row[20]) : 0;
    record->party_user_count = row[21] ? atoi(row[21]) : 0;
    record->rank = row[22] ? atoi(row[22]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonCharac_GetAll(DBConnectionManager* manager, LogDungeonCharac* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[23];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, charac_job, charac_grow, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, exp_avg, dungeon_standard_level, fatigue_consume, exp_add, party_user_count, rank FROM log_dungeon_charac");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonCharac));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_diff = row[2] ? atoi(row[2]) : 0;
        records[count].charac_job = row[3] ? atoi(row[3]) : 0;
        records[count].charac_grow = row[4] ? atoi(row[4]) : 0;
        records[count].clear_time = row[5] ? atoi(row[5]) : 0;
        records[count].die_count = row[6] ? atoi(row[6]) : 0;
        records[count].hp_consume = row[7] ? atoi(row[7]) : 0;
        records[count].mp_consume = row[8] ? atoi(row[8]) : 0;
        records[count].hit_count = row[9] ? atoi(row[9]) : 0;
        records[count].hit_per_avg_damage = row[10] ? atoi(row[10]) : 0;
        records[count].hp_recovery = row[11] ? atoi(row[11]) : 0;
        records[count].mp_recovery = row[12] ? atoi(row[12]) : 0;
        records[count].success = row[13] ? atoi(row[13]) : 0;
        if (row[14]) strncpy(records[count].last_time, row[14], sizeof(records[count].last_time) - 1);
        records[count].update_count = row[15] ? atoi(row[15]) : 0;
        records[count].level = row[16] ? atoi(row[16]) : 0;
        records[count].exp_avg = row[17] ? atoi(row[17]) : 0;
        records[count].dungeon_standard_level = row[18] ? atoi(row[18]) : 0;
        records[count].fatigue_consume = row[19] ? atoi(row[19]) : 0;
        records[count].exp_add = row[20] ? atoi(row[20]) : 0;
        records[count].party_user_count = row[21] ? atoi(row[21]) : 0;
        records[count].rank = row[22] ? atoi(row[22]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
