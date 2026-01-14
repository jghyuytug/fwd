#include "log_dungeon_party_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int LogDungeonParty_Add(DBConnectionManager* manager, const LogDungeonParty* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO log_dungeon_party (channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, party_user_count, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, fatigue_consume, exp_add) "
        "VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d)",
        record->channel_no, record->dungeon_index, record->dungeon_diff, record->dungeon_standard_level, record->abuse_party, record->balkun_party, record->party_user_count, record->clear_time, record->die_count, record->hp_consume, record->mp_consume, record->hit_count, record->hit_per_avg_damage, record->hp_recovery, record->mp_recovery, record->success, record->last_time, record->update_count, record->level, record->fatigue_consume, record->exp_add);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonParty_Get(DBConnectionManager* manager, const char* last_time, int channel_no, int dungeon_index, int dungeon_diff, signed char dungeon_standard_level, signed char abuse_party, signed char balkun_party, int party_user_count, int success, LogDungeonParty* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, party_user_count, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, fatigue_consume, exp_add FROM log_dungeon_party WHERE last_time = '%s' AND channel_no = %d AND dungeon_index = %d AND dungeon_diff = %d AND dungeon_standard_level = %d AND abuse_party = %d AND balkun_party = %d AND party_user_count = %d AND success = %d",
        last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, party_user_count, success);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(LogDungeonParty));
    record->channel_no = row[0] ? atoi(row[0]) : 0;
    record->dungeon_index = row[1] ? atoi(row[1]) : 0;
    record->dungeon_diff = row[2] ? atoi(row[2]) : 0;
    record->dungeon_standard_level = row[3] ? atoi(row[3]) : 0;
    record->abuse_party = row[4] ? atoi(row[4]) : 0;
    record->balkun_party = row[5] ? atoi(row[5]) : 0;
    record->party_user_count = row[6] ? atoi(row[6]) : 0;
    record->clear_time = row[7] ? atoi(row[7]) : 0;
    record->die_count = row[8] ? atoi(row[8]) : 0;
    record->hp_consume = row[9] ? atoi(row[9]) : 0;
    record->mp_consume = row[10] ? atoi(row[10]) : 0;
    record->hit_count = row[11] ? atoi(row[11]) : 0;
    record->hit_per_avg_damage = row[12] ? atoi(row[12]) : 0;
    record->hp_recovery = row[13] ? atoi(row[13]) : 0;
    record->mp_recovery = row[14] ? atoi(row[14]) : 0;
    record->success = row[15] ? atoi(row[15]) : 0;
    if (row[16]) strncpy(record->last_time, row[16], sizeof(record->last_time) - 1);
    record->update_count = row[17] ? atoi(row[17]) : 0;
    record->level = row[18] ? atoi(row[18]) : 0;
    record->fatigue_consume = row[19] ? atoi(row[19]) : 0;
    record->exp_add = row[20] ? atoi(row[20]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int LogDungeonParty_GetAll(DBConnectionManager* manager, LogDungeonParty* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[21];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, party_user_count, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, success, last_time, update_count, level, fatigue_consume, exp_add FROM log_dungeon_party");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN_LOG, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(LogDungeonParty));
        records[count].channel_no = row[0] ? atoi(row[0]) : 0;
        records[count].dungeon_index = row[1] ? atoi(row[1]) : 0;
        records[count].dungeon_diff = row[2] ? atoi(row[2]) : 0;
        records[count].dungeon_standard_level = row[3] ? atoi(row[3]) : 0;
        records[count].abuse_party = row[4] ? atoi(row[4]) : 0;
        records[count].balkun_party = row[5] ? atoi(row[5]) : 0;
        records[count].party_user_count = row[6] ? atoi(row[6]) : 0;
        records[count].clear_time = row[7] ? atoi(row[7]) : 0;
        records[count].die_count = row[8] ? atoi(row[8]) : 0;
        records[count].hp_consume = row[9] ? atoi(row[9]) : 0;
        records[count].mp_consume = row[10] ? atoi(row[10]) : 0;
        records[count].hit_count = row[11] ? atoi(row[11]) : 0;
        records[count].hit_per_avg_damage = row[12] ? atoi(row[12]) : 0;
        records[count].hp_recovery = row[13] ? atoi(row[13]) : 0;
        records[count].mp_recovery = row[14] ? atoi(row[14]) : 0;
        records[count].success = row[15] ? atoi(row[15]) : 0;
        if (row[16]) strncpy(records[count].last_time, row[16], sizeof(records[count].last_time) - 1);
        records[count].update_count = row[17] ? atoi(row[17]) : 0;
        records[count].level = row[18] ? atoi(row[18]) : 0;
        records[count].fatigue_consume = row[19] ? atoi(row[19]) : 0;
        records[count].exp_add = row[20] ? atoi(row[20]) : 0;
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
